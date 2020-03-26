#include "NilfgaardSoldier.h"

void NilfgaardSoldier::Start()
{
	type = Enemy::EnemyType::NILFGAARD_SOLDIER;
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);

	Enemy::StartEnemy();
}

void NilfgaardSoldier::SetStats(const char* json)
{
	std::string json_path = std::string("Configuration/") + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());

	JSONArraypack* stat_weapon = stat->GetArray("Weapons");;
	if (stat_weapon)
	{
		stat_weapon->GetFirstNode();
		stats.health = stat->GetNumber("Health");
		stats.agility = stat->GetNumber("Agility");
		stats.damage = stat->GetNumber("Damage");
		stats.attack_speed = stat->GetNumber("AttackSpeed");

		stat_weapon->GetAnotherNode();
	}

	JSONfilepack::FreeJSON(stat);
}

void NilfgaardSoldier::Update()
{
}

void NilfgaardSoldier::CleanUp()
{
}
