#include "NilfSoldierRange.h"
#include "PlayerController.h"
#include "EnemyManager.h"
#include "ArrowScript.h"
#include "MusicController.h"

NilfSoldierRange::NilfSoldierRange() : NilfgaardSoldier()
{
}

NilfSoldierRange::~NilfSoldierRange()
{
}

void NilfSoldierRange::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue())
			state = NilfgaardSoldierState::MOVE;
		else if (distance < stats["FleeRange"].GetValue())
			state = NilfgaardSoldierState::AUXILIAR;
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
		if (distance < stats["FleeRange"].GetValue())
			state = NilfgaardSoldierState::AUXILIAR;
		break;

	case NilfgaardSoldierState::ATTACK:
		RotateSoldier();
		break;

	case NilfgaardSoldierState::AUXILIAR:
	{
		current_flee_distance = transform->GetGlobalPosition().LengthSq();
		LOG("Last flee distance: %f", last_flee_distance);
		LOG("Current flee distance: %f", current_flee_distance);
		if (math::Abs(last_flee_distance - current_flee_distance) < flee_min_distance)
		{
			Action();
			LOG("Soy un malote");
		}
		Flee();
	}
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

void NilfSoldierRange::CheckDistance()
{
	if ((distance < stats["AttackRange"].GetValue()) && distance > stats["FleeRange"].GetValue())
	{
		animator->SetFloat("speed", 0.0F);
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		Action();
	}
	else if (distance < stats["FleeRange"].GetValue())
	{
		state = NilfgaardSoldierState::AUXILIAR;
		last_flee_distance = 0.0f;
	}
	else if (distance > stats["VisionRange"].GetValue())
	{
		state = NilfgaardSoldierState::IDLE;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		is_combat = false;
	}
}

void NilfSoldierRange::Action()
{
	animator->PlayState("Shoot");
	animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
	state = NilfgaardSoldierState::ATTACK;
}

void NilfSoldierRange::Flee()
{
	float3 velocity_vec = -direction * stats["Agility"].GetValue();
	character_ctrl->Move(velocity_vec * Time::GetDT() * Time::GetScaleTime());
	animator->SetFloat("speed", stats["Agility"].GetValue());

	float angle = atan2f(-direction.z, -direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

	if (distance > stats["RecoverRange"].GetValue())
	{
		state = NilfgaardSoldierState::ATTACK;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		Action();
	}

	last_flee_distance = current_flee_distance;
}

void NilfSoldierRange::ShootAttack()
{
	SpawnAttackParticle();
	float3 arrow_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(arrow, arrow_pos);
	ComponentRigidBody* arrow_rb = arrow_go->GetComponent<ComponentRigidBody>();
	audio_emitter->StartSound("SoldierShoot");
	arrow_go->GetComponent<ArrowScript>()->damage = stats["Damage"].GetValue();
	arrow_rb->SetRotation(RotateProjectile());
	arrow_rb->AddForce(direction.Mul(20), ForceMode::IMPULSE);
}
