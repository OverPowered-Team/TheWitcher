#include "NilfgaardSoldier.h"

//void NilfgaardSoldier::Start()
//{
//	type = Enemy::EnemyType::NILFGAARD_SOLDIER;
//
//	Enemy::Start();
//}

//void NilfgaardSoldier::SetStats(const char* json)
//{
//	//todo handfle array json
//	std::string json_path = std::string("Configuration/") + std::string(json) + std::string(".json");
//	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());
//
//	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
//
//	JSONArraypack* stat_weapon = stat->GetArray("Weapons");;
//	if (stat_weapon)
//	{
//		stat_weapon->GetFirstNode();
//		stats.health = stat->GetNumber("Health");
//		stats.agility = stat->GetNumber("Agility");
//		stats.damage = stat->GetNumber("Damage");
//		stats.attack_speed = stat->GetNumber("AttackSpeed");
//
//		stat_weapon->GetAnotherNode();
//	}
//
//	JSONfilepack::FreeJSON(stat);
//}

void NilfgaardSoldier::Move(float3 direction)
{
	rb->AddForce(direction * stats.agility);
}

void NilfgaardSoldier::Update()
{
	float distance_1 = player_1->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_1->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_2->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_2->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance = (distance_1 < distance_2) ? distance_1 : distance_2;
	float3 direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();

	switch (state)
	{
	case Enemy::EnemyState::IDLE:
		//1.Enemy is In Idle or patroling
		if (distance < stats.vision_range)
		{
			state = Enemy::EnemyState::MOVE;
		}
		break;
	case Enemy::EnemyState::MOVE:
		Move(direction);
		if (distance < stats.attack_range)
		{
			state = Enemy::EnemyState::ATTACK;
		}
		break;
	case Enemy::EnemyState::ATTACK:
		//3.Enemy attacks
		// Check which weapon the enemy has and make one of the attack
		break;
	case Enemy::EnemyState::BLOCK:
		//2.Enemy is blocking
		if (true/*can attack*/)
		{
			state = Enemy::EnemyState::ATTACK;
		}
		break;
	case Enemy::EnemyState::DEAD:
		break;
	case Enemy::EnemyState::FLEE:
		break;
	default:
		break;
	}

}

void NilfgaardSoldier::CleaningUp()
{
}
