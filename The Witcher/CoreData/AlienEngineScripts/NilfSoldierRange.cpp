#include "NilfSoldierRange.h"
#include "EnemyManager.h"
#include "ArrowScript.h"

NilfSoldierRange::NilfSoldierRange() : NilfgaardSoldier()
{
}

NilfSoldierRange::~NilfSoldierRange()
{
}

void NilfSoldierRange::UpdateEnemy()
{
	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue()) {
			state = NilfgaardSoldierState::MOVE;
		}
		else if (distance < stats["FleeRange"].GetValue())
			state = NilfgaardSoldierState::AUXILIAR;
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
		break;

	case NilfgaardSoldierState::ATTACK:
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		transform->SetGlobalRotation(rot);
		break;

	case NilfgaardSoldierState::AUXILIAR:
		Flee();
		break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		audio_emitter->StartSound("SoldierDeath");
		break;
	}
	}
}

void NilfSoldierRange::Action()
{
	animator->PlayState("Shoot");
	state = NilfgaardSoldierState::ATTACK;
}

void NilfSoldierRange::Flee()
{
	float3 velocity_vec = direction * stats["Agility"].GetValue();
	character_ctrl->Move(velocity_vec * Time::GetDT() * Time::GetScaleTime());
	animator->SetFloat("speed", stats["Agility"].GetValue());

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

	if (distance > stats["FleeRange"].GetMaxValue())
	{
		state = NilfgaardSoldierState::ATTACK;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		Action();
	}
}

void NilfSoldierRange::ShootAttack()
{
	float3 arrow_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(arrow, arrow_pos);
	ComponentRigidBody* arrow_rb = arrow_go->GetComponent<ComponentRigidBody>();
	audio_emitter->StartSound("SoldierShoot");
	arrow_go->GetComponent<ArrowScript>()->damage = stats["Damage"].GetValue();
	arrow_rb->SetRotation(RotateArrow());
	arrow_rb->AddForce(direction.Mul(20), ForceMode::IMPULSE);
}

Quat NilfSoldierRange::RotateArrow()
{
	float3 front = -float3::unitZ(); //front of the object
	Quat rot1 = Quat::RotateFromTo(front, direction);

	float3 desiredUp = float3::unitY();
	float3 right = Cross(direction, desiredUp);
	desiredUp = Cross(right, direction);

	float3 newUp = rot1 * float3(0.0f, 1.0f, 0.0f);
	Quat rot2 = Quat::RotateFromTo(newUp, desiredUp);
	return rot2 * rot1;
}
