#include "DrownedRange.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
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
			animator->SetFloat("speed", 0.0F);
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
			animator->PlayState("GetOff");
			state = DrownedState::GETOFF;
			is_hide = false;
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

	case DrownedState::GETOFF:
		if (movement < 1)
		{
			character_ctrl->Move(float3::unitY() * 0.01);
			movement += 0.01;
		}
		else
		{
			movement = 0;
			state = DrownedState::ATTACK;
			animator->PlayState("Attack");
		}
		break;

	case DrownedState::ATTACK:
		if (set_attack)
		{
			animator->PlayState("Attack");
			set_attack = false;
		}
		break;

	case DrownedState::HIDE:
		if (movement < 1)
		{
			character_ctrl->Move(-float3::unitY() * 0.01);
			movement += 0.01;
		}
		if (Time::GetGameTime() - current_hide_time > max_hide_time)
		{
			movement = 0;
			animator->PlayState("Hide");
			state = DrownedState::IDLE;
		}
		break;

	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = DrownedState::DEAD;
		animator->PlayState("Dead");
		last_player_hit->OnEnemyKill();
		audio_emitter->StartSound("Play_Drowner_Death");
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
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
			is_hide = true;
		}
		can_get_interrupted = true;
		stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
		set_attack = true;
		movement = 0;
	}
	else if (strcmp(name, "Hit") == 0 && state != DrownedState::DEAD) {
		state = DrownedState::ATTACK;
		set_attack = true;
		movement = 0;
	}
}
