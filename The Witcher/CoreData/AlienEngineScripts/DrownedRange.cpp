#include "DrownedRange.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "MusicController.h"
#include "ArrowScript.h"

DrownedRange::DrownedRange() : Drowned()
{
}

DrownedRange::~DrownedRange()
{
}

void DrownedRange::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	switch (state)
	{
	case DrownedState::IDLE:
		if (distance < stats["AttackRange"].GetValue() && distance > stats["HideDistance"].GetValue())
		{
			SetState("GetOff");
			animator->PlayState("GetOff");
			if (m_controller && !is_combat)
			{
				is_combat = true;
				m_controller->EnemyInSight((Enemy*)this);
			}
		}
		else {
			if (m_controller && is_combat)
			{
				is_combat = false;
				m_controller->EnemyLostSight((Enemy*)this);
			}
		}
		break;

	case DrownedState::IDLE_OUT:
		if (distance < stats["AttackRange"].GetValue() && distance > stats["HideDistance"].GetValue())
		{
			SetState("Attack");
			animator->PlayState("Attack");
		}
		break;

	case DrownedState::ATTACK:
	{
		RotatePlayer();
	}
		break;

	case DrownedState::HIDE:
	{
		if (Time::GetGameTime() - current_hide_time > max_hide_time)
		{
			SetState("Idle");
			is_hide = true;
		}
	}
	break;

	case DrownedState::HIT:
	{
		velocity += velocity * knock_slow * Time::GetDT();
		velocity.y += gravity * Time::GetDT();
		character_ctrl->Move(velocity * Time::GetDT());
	}
		break;

	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = DrownedState::DEAD;
		animator->PlayState("Death");
		last_player_hit->OnEnemyKill((uint)type);
		audio_emitter->StartSound("Play_Drowner_Death");
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
		if (is_obstacle)
		{
			game_object->parent->parent->GetComponent<BlockerObstacle>()->ReleaseMyself(this);
		}
	}
	}
}

void DrownedRange::ShootSlime()
{
	float3 slime_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(slime, slime_pos);
	ComponentRigidBody* arrow_rb = arrow_go->GetComponent<ComponentRigidBody>();
	audio_emitter->StartSound("Play_Drowner_Shot_Attack");
	arrow_go->GetComponent<ArrowScript>()->damage = stats["Damage"].GetValue();
	arrow_rb->SetRotation(RotateProjectile());
	arrow_rb->AddForce(direction.Mul(20), ForceMode::IMPULSE);
}

void DrownedRange::OnAnimationEnd(const char* name)
{
	if (strcmp(name, "Attack") == 0) {
		if (distance < stats["HideDistance"].GetValue() || distance > stats["AttackRange"].GetValue())
		{
			state = DrownedState::HIDE;
			current_hide_time = Time::GetGameTime();
			animator->SetBool("hide", true);
		}
		else
		{
			SetState("IdleOut");
			animator->SetBool("hide", false);
		}
		can_get_interrupted = true;
		//stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		//animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
	}
	else if (strcmp(name, "Hit") == 0) {
		ReleaseParticle("hit_particle");
		if (!is_dead)
		{
			state = DrownedState::ATTACK;
		}
		else
		{
			if(!was_dizzy)
				was_dizzy = true;
			else
			{
				state = DrownedState::DYING;
				GameManager::instance->player_manager->IncreaseUltimateCharge(10);
			}
		}
			
	}
	else if ((strcmp(name, "Dizzy") == 0) && stats["Health"].GetValue() <= 0)
	{
		state = DrownedState::DYING;
		GameManager::instance->player_manager->IncreaseUltimateCharge(10);
	}
	else if (strcmp(name, "GetOff") == 0)
	{
		SetState("Attack");
	}
}
