#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::StartEnemy()
{
	state = EnemyState::IDLE;
	std::string json_str = "";

	switch (type)
	{
	case Enemy::EnemyType::NONE:
		json_str = "None";
		break;
	case Enemy::EnemyType::GHOUL:
		json_str = "ghoul";
		break;
	case Enemy::EnemyType::NILFGAARD_SOLDIER:
		json_str = "nilfgaardsoldier";
		break;
	default:
		break;
	}

	SetStats(json_str.data());
}

void Enemy::SetStats(const char* json)
{
	std::string json_path = std::string("Configuration/") + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
	if (stat)
	{
		stats.health = stat->GetNumber("Health");
		stats.agility = stat->GetNumber("Agility");
		stats.damage = stat->GetNumber("Damage");
		stats.attack_speed = stat->GetNumber("AttackSpeed");
	}

	JSONfilepack::FreeJSON(stat);
}
