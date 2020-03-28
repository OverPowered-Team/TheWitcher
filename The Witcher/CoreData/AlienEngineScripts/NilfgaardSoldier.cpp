#include "NilfgaardSoldier.h"
#include "ArrowMovement.h"

void NilfgaardSoldier::Start()
{
	type = EnemyType::NILFGAARD_SOLDIER;

	Enemy::Start();
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

		stats.health = stat_weapon->GetNumber("Health");
		stats.agility = stat_weapon->GetNumber("Agility");
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
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());

	//Quat current_rot = math::Slerp() TODO: Slerp between char current rot and desired rot
	character_ctrl->SetRotation(rot);
	character_ctrl->SetWalkDirection(direction * stats.agility);
	animator->SetFloat("speed", stats.agility);

	if (distance < stats.attack_range)
	{
		state = Enemy::EnemyState::ATTACK;
		character_ctrl->SetWalkDirection(float3(0.0F, 0.0F, 0.0F));
		animator->SetFloat("speed", 0.0F);
		animator->PlayState("Attack");
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
		break;
	case NilfgaardSoldier::NilfgaardType::ARCHER:
		ShootAttack();
		break;
	}
}

void NilfgaardSoldier::ShootAttack()
{
	float3 arrow_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.5F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(arrow, arrow_pos);

	ArrowMovement* arrow_mov = (ArrowMovement*)arrow_go->GetComponentScript("ArrowMovement");
	arrow_mov->direction = direction;
}

void NilfgaardSoldier::Update()
{
	float distance_1 = player_1->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_1->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_2->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_2->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	distance = (distance_1 < distance_2) ? distance_1 : distance_2;
	direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();

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
		break;
	case Enemy::EnemyState::DEAD:
		break;
	case Enemy::EnemyState::FLEE:
		break;
	default:
		break;
	}

}

void NilfgaardSoldier::CleanUp()
{
}

void NilfgaardSoldier::OnAnimationEnd(const char* name) {

	if (strcmp(name, "Attack") == 0) {
		if (distance < stats.vision_range)
		{
			state = Enemy::EnemyState::MOVE;
		}
		else
		{
			state = Enemy::EnemyState::IDLE;
		}
	}
}
