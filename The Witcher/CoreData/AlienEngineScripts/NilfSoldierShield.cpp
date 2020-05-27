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

void NilfSoldierShield::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue())
			state = NilfgaardSoldierState::MOVE;
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
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
			state = NilfgaardSoldierState::IDLE;
		}
		break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		animator->PlayState("Death");
		audio_emitter->StartSound("SoldierDeath");
		last_player_hit->OnEnemyKill();
		state = NilfgaardSoldierState::DEAD;
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
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
	float b_time = (has_been_attacked) ? block_attack_time : block_time;
	if (Time::GetGameTime() - current_time > b_time)
	{
		ReleaseParticle("ClinckEmitter");

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
		ReleaseParticle("ClinckEmitter");

		state = NilfgaardSoldierState::HIT;
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

void NilfSoldierShield::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != NilfgaardSoldierState::DEAD) {

		if (is_blocked && CheckPlayerForward())
		{
			has_been_attacked = true;
			current_time = Time::GetGameTime();
			break_shield_attack++;
			SpawnParticle("ClinckEmitter", particle_spawn_positions[4]->transform->GetLocalPosition()); // 1 is body position
			audio_emitter->StartSound("SoldierBlock");
		}
		else
		{
			PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
			if (player && player->attacks->GetCurrentAttack()->CanHit(this))
			{
				float dmg_received = player->attacks->GetCurrentDMG();
				player->OnHit(this, GetDamaged(dmg_received, player));
				last_player_hit = player;

				HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
			}
		}
	}
}