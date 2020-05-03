#include "Drowned.h"
#include "PlayerController.h"

Drowned::Drowned() : Enemy()
{
}

Drowned::~Drowned()
{
}

void Drowned::StartEnemy()
{
	type = EnemyType::DROWNED;
	state = DrownedState::IDLE;
	Enemy::StartEnemy();
}

void Drowned::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());

	JSONArraypack* stat_weapon = stat->GetArray("Drowned");
	int i = 0;
	if (stat_weapon)
	{
		stat_weapon->GetFirstNode();
		for (int i = 0; i < stat_weapon->GetArraySize(); ++i)
			if (stat_weapon->GetNumber("Type") != (int)drowned_type)
				stat_weapon->GetAnotherNode();
			else
				break;

		stats["Health"] = Stat("Health", stat_weapon->GetNumber("Health"));
		stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("Agility"));
		stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("Damage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("AttackSpeed"));
		stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
		stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));
		stats["HideDistance"] = Stat("HideDistance", stat_weapon->GetNumber("HideDistance"));

		if (drowned_type == DrownedType::RANGE)
			stats["GetOffRange"] = Stat("GetOffRange", stat_weapon->GetNumber("GetOffRange"));
	/*	else if (drowned_type == DrownedType::GRAB)
			stats["BlockRange"] = Stat("BlockRange", stat_weapon->GetNumber("BlockRange"));*/

		stat_weapon->GetAnotherNode();
	}

	JSONfilepack::FreeJSON(stat);
}

float Drowned::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	float damage = Enemy::GetDamaged(dmg, player);

	if (can_get_interrupted) {
		state = DrownedState::HIT;
		animator->PlayState("Hit");
	}

	//audio_emitter->StartSound("GhoulHit");
	if (particles["hit_particle"])
		particles["hit_particle"]->Restart();

	character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

	if (stats["Health"].GetValue() == 0.0F) {

		animator->SetBool("dead", true);
		OnDeathHit();
		state = DrownedState::DYING;
		//audio_emitter->StartSound("GhoulDeath");
		player->OnEnemyKill();
	}

	return damage;
}

void Drowned::Stun(float time)
{
	if (state != DrownedState::STUNNED)
	{
		state = DrownedState::STUNNED;
		animator->PlayState("Dizzy");
		current_stun_time = Time::GetGameTime();
		stun_time = time;
	}
}

bool Drowned::IsDead()
{
	return (state == DrownedState::DEAD ? true : false);
}

