#include "Enemy.h"
#include "EnemyManager.h"

void Enemy::Awake()
{
	//((EnemyManager*)(GameObject::FindWithName("EnemyManager")->GetComponentScript("EnemyManager")))->AddEnemy(this);
}

void Enemy::StartEnemy()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	character_ctrl = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
	state = EnemyState::IDLE;
	std::string json_str;

	//player_1 = TODO: Find gameObject of player 1
	//player_2 = TODO: Find gameObject of player 2

	switch (type)
	{
	case EnemyType::NONE:
		json_str = "None";
		break;
	case EnemyType::GHOUL:
		json_str = "ghoul";
		break;
	case EnemyType::NILFGAARD_SOLDIER:
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
		stats.attack_range = stat->GetNumber("AttackRange");
		stats.vision_range = stat->GetNumber("VisionRange");
	}

	JSONfilepack::FreeJSON(stat);
}

