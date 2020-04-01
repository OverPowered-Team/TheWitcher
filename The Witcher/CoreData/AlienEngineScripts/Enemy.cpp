#include "Enemy.h"
#include "EnemyManager.h"
#include "PlayerAttacks.h"

void Enemy::Awake()
{
	((EnemyManager*)(GameObject::FindWithName("GameManager")->GetComponentScript("EnemyManager")))->AddEnemy(this);
	collider = (ComponentCollider*)game_object->GetChild("EnemyAttack")->GetComponent(ComponentType::BOX_COLLIDER);
}

void Enemy::StartEnemy()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	character_ctrl = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
	state = EnemyState::IDLE;
	std::string json_str;

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
		stats.max_health = stats.current_health = stat->GetNumber("Health");
		stats.agility = stat->GetNumber("Agility");
		stats.damage = stat->GetNumber("Damage");
		stats.attack_speed = stat->GetNumber("AttackSpeed");
		stats.attack_range = stat->GetNumber("AttackRange");
		stats.vision_range = stat->GetNumber("VisionRange");
	}

	JSONfilepack::FreeJSON(stat);
}

void Enemy::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0) {
		PlayerAttacks* player_attacks = static_cast<PlayerAttacks*>(collider->game_object_attached->GetComponentScriptInParent("PlayerAttacks"));
		float dmg_recieved = player_attacks->GetCurrentDMG();
		player_attacks->OnHit(this);
		GetDamaged(dmg_recieved);
	}
}

void Enemy::GetDamaged(float dmg)
{
	stats.current_health -= dmg;
	if (stats.current_health <= 0.0F) {
		stats.current_health = 0.0F;
		state = EnemyState::DEAD;
	}
}

void Enemy::ActivateCollider()
{
	if (collider)
	{
		collider->SetEnable(true);
	}
}

void Enemy::DeactivateCollider()
{
	if (collider)
	{
		collider->SetEnable(false);
	}
}

