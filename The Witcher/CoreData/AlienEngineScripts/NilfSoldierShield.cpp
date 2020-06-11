#include "GameManager.h"
#include "ParticlePool.h"
#include "NilfSoldierShield.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "MusicController.h"

NilfSoldierShield::NilfSoldierShield() : NilfgaardSoldier()
{
}

NilfSoldierShield::~NilfSoldierShield()
{
}

void NilfSoldierShield::StartEnemy()
{
	can_get_interrupted = false;
	NilfgaardSoldier::StartEnemy();

	particle_spawn_positions.push_back(game_object->GetChildRecursive("Shield_Position"));
}

void NilfSoldierShield::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue() || is_obstacle)
			state = NilfgaardSoldierState::MOVE;
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
		break;

	case NilfgaardSoldierState::HIT:
	{
		velocity += velocity * knock_slow * Time::GetDT();
		velocity.y += gravity * Time::GetDT();
		character_ctrl->Move(velocity * Time::GetDT());
	}
	break;

	case NilfgaardSoldierState::AUXILIAR:
		if (stats["BlockRange"].GetValue() < distance)
		{
			state = NilfgaardSoldierState::IDLE;
			animator->PlayState("Idle");
		}
		Block();
		break;

	case NilfgaardSoldierState::GUARD:
		Guard();
		break;

	case NilfgaardSoldierState::STUNNED:
		if (Time::GetGameTime() - current_stun_time > stun_time)
		{
			SetState("Idle");
			animator->PlayState("Idle");
			animator->SetBool("stunned", false);
		}
		break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		animator->PlayState("Death");
		audio_emitter->StartSound("SoldierDeath");
		last_player_hit->OnEnemyKill((uint)type);
		state = NilfgaardSoldierState::DEAD;
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
		if (is_obstacle)
		{
			game_object->parent->parent->GetComponent<BlockerObstacle>()->ReleaseMyself(this);
		}
		break;
	}
	}
}

void NilfSoldierShield::Action()
{
	int rand_num = Random::GetRandomIntBetweenTwo(0, 1);
	if (rand_num == 0)
	{
		SetState("Block");
		current_time = Time::GetGameTime();
		is_blocked = true;
	}
	else
	{
		animator->PlayState("Attack");
		animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
		state = NilfgaardSoldierState::ATTACK;
	}
}

void NilfSoldierShield::Block()
{
	RotateToPlayerSmooth(0.9f);
	float b_time = (has_been_attacked) ? block_attack_time : block_time;
	if (Time::GetGameTime() - current_time > b_time)
	{
		ReleaseParticle("BlinkEmitter");

		if (stats["AttackRange"].GetValue() < distance)
		{
			state = NilfgaardSoldierState::IDLE;
			animator->PlayState("Idle");
		}
		else
		{
			state = NilfgaardSoldierState::ATTACK;
			animator->PlayState("Attack");
		}

		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
	else if (break_shield_attack >= max_break_shield_attack)
	{
		ReleaseParticle("BlinkEmitter");

		Stun(5.0f);
		animator->PlayState("Hit");
		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
}

bool NilfSoldierShield::CheckPlayerForward()
{
	float angle = transform->forward.AngleBetween(player_controllers[current_player]->transform->forward) * RADTODEG;
	LOG("Angle: %f", angle);
	if (angle > 100 && angle < 200)
		return true;
	else
		return false;
}

float NilfSoldierShield::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	float damage = 0.0f;

	if (is_blocked && CheckPlayerForward())
	{
		has_been_attacked = true;
		current_time = Time::GetGameTime();
		if(player->attacks->GetCurrentAttack()->IsLast() || player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Spell))
			break_shield_attack += 2;
		else 
			break_shield_attack++;

		SpawnParticle("BlinkEmitter", particle_spawn_positions[4]->transform->GetGlobalPosition());
		PlaySFX("Block");
	}
	else
	{
		if (!CheckPlayerForward())
		{
			if(state != NilfgaardSoldierState::STUNNED)
				animator->PlayState("Hit");

			Stun(5.0f);
			has_been_attacked = false;
			break_shield_attack = 0;
			is_blocked = false;
		}

		float aux_health = stats["Health"].GetValue();
		stats["Health"].DecreaseStat(dmg);

		last_player_hit = player;
		velocity = knock_back; 

		SpawnParticle(last_player_hit->attacks->GetCurrentAttack()->info.hit_particle_name, particle_spawn_positions[1]->transform->GetGlobalPosition(), false, float3(0.0f, 0.0f, -20.0f));
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

		if (stats["Health"].GetValue() <= 0.0F) {

			animator->SetBool("dead", true);
			is_dead = true;
			OnDeathHit();

			if (player->attacks->GetCurrentAttack() && player->attacks->GetCurrentAttack()->IsLast())
			{
				SetState("Dying");
				PlaySFX("Death");

				Decapitate(player);
			}
		}

		HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);

		damage = aux_health - stats["Health"].GetValue();
	}
	return damage;
}
