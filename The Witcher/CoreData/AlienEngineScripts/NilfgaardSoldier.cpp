#include "NilfgaardSoldier.h"
#include "ArrowScript.h"
#include "PlayerController.h"
#include "EnemyManager.h"

void NilfgaardSoldier::StartEnemy()
{
	type = EnemyType::NILFGAARD_SOLDIER;

	Enemy::StartEnemy();
}

void NilfgaardSoldier::SetStats(const char* json)
{
	//todo handfle array json
	std::string json_path = std::string("Configuration/") + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());

	JSONArraypack* stat_weapon = stat->GetArray("Weapons");
	int i = 0;
	if (stat_weapon)
	{
		stat_weapon->GetFirstNode();

		for (int i = 0; i < stat_weapon->GetArraySize(); ++i)
			if (stat_weapon->GetNumber("Type") != (int)nilf_type)
				stat_weapon->GetAnotherNode();
			else
				break;

		stats.max_health = stats.current_health = stat_weapon->GetNumber("Health");
		stats.agility = Stat("Agility", stat_weapon->GetNumber("Agility"));
		stats.damage = stat_weapon->GetNumber("Damage");
		stats.attack_speed = stat_weapon->GetNumber("AttackSpeed");
		stats.vision_range = stat_weapon->GetNumber("VisionRange");
		stats.attack_range = stat_weapon->GetNumber("AttackRange");

		stat_weapon->GetAnotherNode();
	}

	JSONfilepack::FreeJSON(stat);
}

void NilfgaardSoldier::Move(float3 direction)
{
	character_ctrl->SetWalkDirection(direction * stats.agility.GetValue());
	animator->SetFloat("speed", stats.agility.GetValue());

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	character_ctrl->SetRotation(rot);

	if (distance < stats.attack_range)
	{
		state = Enemy::EnemyState::ATTACK;
		character_ctrl->SetWalkDirection(float3(0.0F, 0.0F, 0.0F));
		animator->SetFloat("speed", 0.0F);
		Attack();
	}
	if (distance > stats.vision_range)
	{
		state = Enemy::EnemyState::IDLE;
		character_ctrl->SetWalkDirection(float3(0.0F, 0.0F, 0.0F));
		animator->SetFloat("speed", 0.0F);
	}
}

void NilfgaardSoldier::Attack()
{
	switch (nilf_type)
	{
	case NilfgaardSoldier::NilfgaardType::SWORD_SHIELD:
		animator->PlayState("Attack");
		break;
	case NilfgaardSoldier::NilfgaardType::ARCHER:
		animator->PlayState("Shoot");
		break;
	}
}

void NilfgaardSoldier::ShootAttack()
{
	float3 arrow_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(arrow, arrow_pos);
	ComponentRigidBody* arrow_rb = (ComponentRigidBody*)arrow_go->GetComponent(ComponentType::RIGID_BODY);
	static_cast<ArrowScript*>(arrow_go->GetChild("Point")->GetComponentScript("ArrowScript"))->damage = stats.damage;
	arrow_rb->SetRotation(RotateArrow());
	arrow_rb->AddForce(direction.Mul(20));
}

Quat NilfgaardSoldier::RotateArrow()
{
	float3 front = float3::unitZ(); //front of the object
	Quat rot1 = Quat::RotateFromTo(front, direction);

	float3 desiredUp = float3::unitY();
	float3 right = Cross(direction, desiredUp);
	desiredUp = Cross(right, direction);

	float3 newUp = rot1 * float3(0.0f, 1.0f, 0.0f);
	Quat rot2 = Quat::RotateFromTo(newUp, desiredUp);
	return rot2 * rot1;
}

void NilfgaardSoldier::UpdateEnemy()
{	
	float distance_1 = player_1->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_1->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_2->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_2->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	if (player_controllers[0]->state == PlayerController::PlayerState::DEAD)
	{
		distance = distance_2;
		direction = direction_2.Normalized();
	}
	else if (player_controllers[1]->state == PlayerController::PlayerState::DEAD)
	{
		distance = distance_1;
		direction = direction_1.Normalized();
	}
	else
	{
		distance = (distance_1 < distance_2) ? distance_1 : distance_2;
		direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();
	}

	switch (state)
	{
	case Enemy::EnemyState::IDLE:
		if (distance < stats.vision_range)
			state = Enemy::EnemyState::MOVE;
		break;
	case Enemy::EnemyState::MOVE:
		Move(direction);
		break;
	case Enemy::EnemyState::ATTACK:
		switch (nilf_type)
		{
		case NilfgaardSoldier::NilfgaardType::ARCHER:
			float angle = atan2f(direction.z, direction.x);
			Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
			character_ctrl->SetRotation(rot);
			break;
		}
		break;
	case Enemy::EnemyState::HIT:

		break;
	case Enemy::EnemyState::DYING:
	{
		EnemyManager* enemy_manager = (EnemyManager*)(GameObject::FindWithName("GameManager")->GetComponentScript("EnemyManager"));
		//Ori Ori function sintaxis
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = EnemyState::DEAD;
		break;
	}
	case Enemy::EnemyState::DEAD:
		break;
	default:
		break;
	}

	Enemy::UpdateEnemy();

}

void NilfgaardSoldier::CleanUpEnemy()
{
}

void NilfgaardSoldier::OnAnimationEnd(const char* name) {

	if (strcmp(name, "Attack") == 0 || strcmp(name, "Shoot") == 0) {
		if (distance < stats.vision_range)
		{
			state = Enemy::EnemyState::MOVE;
		}
		else
		{
			state = Enemy::EnemyState::IDLE;
		}
	}

	if (strcmp(name, "Hit") == 0) {
		state = Enemy::EnemyState::IDLE;
	}
}
