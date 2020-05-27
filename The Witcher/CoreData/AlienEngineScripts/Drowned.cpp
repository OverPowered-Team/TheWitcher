#include "Drowned.h"
#include "MusicController.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"

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
	m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
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

		stats["Health"] = Stat("Health", stat_weapon->GetNumber("MinHealth"), stat_weapon->GetNumber("Health"), stat_weapon->GetNumber("MaxHealth"));
		stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("MinAgility"), stat_weapon->GetNumber("Agility"), stat_weapon->GetNumber("MaxAgility"));
		stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("MinDamage"), stat_weapon->GetNumber("Damage"), stat_weapon->GetNumber("MaxDamage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("MinAttackSpeed"), stat_weapon->GetNumber("AttackSpeed"), stat_weapon->GetNumber("MaxAttackSpeed"));
		stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));
		stats["JumpRange"] = Stat("JumpRange", stat_weapon->GetNumber("JumpAttackRange"));
		stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
		stats["JumpForce"] = Stat("JumpForce", stat_weapon->GetNumber("JumpForce"));
		stats["HitSpeed"] = Stat("HitSpeed", stat_weapon->GetNumber("HitSpeed"));
		stats["HitSpeed"].SetMaxValue(stat_weapon->GetNumber("MaxHitSpeed"));

		stats["GetOffRange"] = Stat("GetOffRange", stat_weapon->GetNumber("GetOffRange"));

		stat_weapon->GetAnotherNode();
	}

	JSONfilepack::FreeJSON(stat);
}

void Drowned::CleanUpEnemy()
{
	ReleaseAllParticles();
}

void Drowned::Stun(float time)
{
	if (state != DrownedState::STUNNED && state != DrownedState::DEAD && state != DrownedState::DYING)
	{
		state = DrownedState::STUNNED;
		animator->PlayState("Dizzy");
		audio_emitter->StartSound("Play_Dizzy_Enemy");
		current_stun_time = Time::GetGameTime();
		stun_time = time;
	}
}

bool Drowned::IsDead()
{
	return (state == DrownedState::DEAD ? true : false);
}

void Drowned::PlaySFX(const char* sfx_name)
{
	if (sfx_name == "Hit")
		audio_emitter->StartSound("Play_Drowner_Hit");
	else if (sfx_name == "Death")
		audio_emitter->StartSound("Play_Drowner_Hit"); // TODO: Put Death sfx
	else
		LOG("Sound effect with name %s not found!", sfx_name);
}

void Drowned::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != DrownedState::DEAD) {
		if (!is_hide)
		{
			PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
			if (player && player->attacks->GetCurrentAttack()->CanHit(this))
			{
				float dmg_received = player->attacks->GetCurrentDMG();
				float3 knock = player->attacks->GetKnockBack(this->transform);
				player->OnHit(this, GetDamaged(dmg_received, player, knock));
				HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
			}
		}
	}
}
