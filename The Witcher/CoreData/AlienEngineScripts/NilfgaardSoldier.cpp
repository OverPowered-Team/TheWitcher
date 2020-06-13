#include "NilfgaardSoldier.h"
#include "ArrowScript.h"
#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "EnemyManager.h"
#include "MusicController.h"

#include "InGame_UI.h"

void NilfgaardSoldier::StartEnemy()
{
	type = EnemyType::NILFGAARD_SOLDIER;
	state = NilfgaardSoldierState::IDLE;
	decapitation_particle = "decapitation_particle_human";
	m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
	Enemy::StartEnemy();
}

void NilfgaardSoldier::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());

	JSONArraypack* stat_weapon = stat->GetArray("NilfgaardSoldier");
	int i = 0;
	if (stat_weapon)
	{
		stat_weapon->GetFirstNode();
		for (int i = 0; i < stat_weapon->GetArraySize(); ++i)
			if (stat_weapon->GetNumber("Type") != (int)nilf_type)
				stat_weapon->GetAnotherNode();
			else
				break;

		stats["Health"] = Stat("Health", stat_weapon->GetNumber("MinHealth"), stat_weapon->GetNumber("Health"), stat_weapon->GetNumber("MaxHealth"));
		stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("MinAgility"), stat_weapon->GetNumber("Agility"), stat_weapon->GetNumber("MaxAgility"));
		stats["Acceleration"] = Stat("Acceleration", stat_weapon->GetNumber("Acceleration"));
		stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("MinDamage"), stat_weapon->GetNumber("Damage"), stat_weapon->GetNumber("MaxDamage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("MinAttackSpeed"), stat_weapon->GetNumber("AttackSpeed"), stat_weapon->GetNumber("MaxAttackSpeed"));
		stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));
		stats["JumpRange"] = Stat("JumpRange", stat_weapon->GetNumber("JumpAttackRange"));
		stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
		stats["JumpForce"] = Stat("JumpForce", stat_weapon->GetNumber("JumpForce"));
		stats["HitSpeed"] = Stat("HitSpeed", stat_weapon->GetNumber("HitSpeed"));
		stats["HitSpeed"].SetMaxValue(stat_weapon->GetNumber("MaxHitSpeed"));

		if (nilf_type == NilfgaardType::ARCHER)
		{
			stats["FleeRange"] = Stat("FleeRange", stat_weapon->GetNumber("FleeRange"));
			stats["RecoverRange"] = Stat("RecoverRange", stat_weapon->GetNumber("RecoverRange"));
		}
		else if (nilf_type == NilfgaardType::SWORD_SHIELD)
			stats["BlockRange"] = Stat("BlockRange", stat_weapon->GetNumber("BlockRange"));

	}

	JSONfilepack::FreeJSON(stat);
}

void NilfgaardSoldier::OnDeathHit()
{
	/*LOG("PARENT IS %s  THIS GO IS %s",weapon_go->parent->GetName(), game_object->GetName())
	if (weapon_go->parent != game_object->parent)
	{
		weapon_go->SetNewParent(this->game_object->parent);
		weapon_go->GetComponent(ComponentType::RIGID_BODY)->SetEnable(true);
		weapon_go->GetComponent(ComponentType::BOX_COLLIDER)->SetEnable(true);
	}*/
}

void NilfgaardSoldier::CheckDistance()
{
	if ((distance < stats["AttackRange"].GetValue()) && is_attacking)
	{
		float angle = atan2f(direction.z, direction.x);
		transform->SetGlobalRotation(Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad()));
		Action();
	}

	if (distance > stats["VisionRange"].GetValue() && !is_obstacle)
	{
		SetState("Idle");
		RemoveBattleCircle();

		if (m_controller && is_combat) {
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
		
	}
	else if(!is_attacking)
	{
		if (m_controller && !is_combat) {
			is_combat = true;
			m_controller->EnemyInSight((Enemy*)this);
		}
	}
}

bool NilfgaardSoldier::IsRangeEnemy()
{
	if (nilf_type == NilfgaardType::ARCHER)
		return true;
	else
		return false;
}

void NilfgaardSoldier::RotateSoldier()
{
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

void NilfgaardSoldier::PlaySFX(const char* sfx_name)
{
	if (sfx_name == "Hit")
		audio_emitter->StartSound("SoldierHit");
	else if (sfx_name == "Death")
		audio_emitter->StartSound("SoldierDeath");
	else if (sfx_name == "Block")
		audio_emitter->StartSound("SoldierBlock");
	else
		LOG("Sound effect with name %s not found!", sfx_name);
}


void NilfgaardSoldier::Stun(float time)
{
	if (state != NilfgaardSoldierState::STUNNED && state != NilfgaardSoldierState::DEAD && state != NilfgaardSoldierState::DYING)
	{
		state = NilfgaardSoldierState::STUNNED;
		animator->SetBool("stunned", true);
		current_stun_time = Time::GetGameTime();
		stun_time = time;
		audio_emitter->StartSound("Play_Dizzy_Enemy");
	}
}

bool NilfgaardSoldier::IsDead()
{
	return (state == NilfgaardSoldierState::DEAD ? true : false);
}
bool NilfgaardSoldier::IsDying()
{
	return (state == NilfgaardSoldierState::DYING ? true : false);
}

bool NilfgaardSoldier::IsHit()
{
	return (state == NilfgaardSoldierState::HIT ? true : false);
}

void NilfgaardSoldier::SetState(const char* state_str)
{
	if (state_str == "Idle")
	{
		state = NilfgaardSoldierState::IDLE;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		velocity = float3::zero();
		animator->SetFloat("speed", 0.0F);
	}
	else if (state_str == "Move")
	{
		state = NilfgaardSoldierState::MOVE;
	}
	else if (state_str == "Attack")
	{
		state = NilfgaardSoldierState::ATTACK;
		animator->SetFloat("speed", 0.0F);
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		velocity = float3::zero();
	}
	else if (state_str == "Guard")
	{
		state = NilfgaardSoldierState::GUARD;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		velocity = float3::zero();
		animator->SetFloat("speed", 0.0F);
	}
	else if (state_str == "Block")
	{
		animator->PlayState("Block");
		//animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
		state = NilfgaardSoldierState::AUXILIAR;
	}
	else if (state_str == "Flee")
	{
		state = NilfgaardSoldierState::AUXILIAR;
	}
	else if (state_str == "Hit")
		state = NilfgaardSoldierState::HIT;
	else if (state_str == "Dying")
		state = NilfgaardSoldierState::DYING;
	else if (state_str == "Dead")
		state = NilfgaardSoldierState::DEAD;
	else if (state_str == "Stunned")
		state = NilfgaardSoldierState::STUNNED;
	else
		LOG("Incorrect state name: %s", state_str);
}

void NilfgaardSoldier::OnAnimationEnd(const char* name) {

	if (strcmp(name, "Attack") == 0 || strcmp(name, "Shoot") == 0) {
		//stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		//animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
		if(nilf_type != NilfgaardType::SWORD_SHIELD)
			can_get_interrupted = true;
		ReleaseParticle("EnemyAttackParticle");

		if (distance < stats["VisionRange"].GetValue())
			SetState("Move");
		else
			SetState("Idle");

	}
	else if (strcmp(name, "Hit") == 0) {
		ReleaseParticle("hit_particle");

		if (nilf_type == NilfgaardType::SWORD_SHIELD)
			return;

		if (stats["Health"].GetValue() <= 0.0F) {
			SetState("Hit");
			if (!IsRangeEnemy())
				RemoveBattleCircle();

			if (!was_dizzy)
				was_dizzy = true;
			else
			{
				state = NilfgaardSoldierState::DYING;
			}
		}
		else if (is_attacking)
		{
			ChangeAttackEnemy();		
		}
		else if (!is_dead) 
		{
			SetState("Guard");
		}

	}
	else if ((strcmp(name, "Dizzy") == 0))
	{
		if (stats["Health"].GetValue() <= 0) 
		{
			state = NilfgaardSoldierState::DYING;
		}
		else
		{
			can_get_interrupted = false;
		}
	}
}

bool NilfgaardSoldier::IsState(const char* state_str)
{
	if (state_str == "Idle")
		return (state == NilfgaardSoldierState::IDLE ? true : false);
	else if (state_str == "Move")
		return (state == NilfgaardSoldierState::MOVE ? true : false);
	else if (state_str == "Attack")
		return (state == NilfgaardSoldierState::ATTACK ? true : false);
	else if (state_str == "Hit")
		return (state == NilfgaardSoldierState::HIT ? true : false);
	else if (state_str == "Dying")
		return (state == NilfgaardSoldierState::DYING ? true : false);
	else if (state_str == "Stunned")
		return (state == NilfgaardSoldierState::STUNNED ? true : false);
	else
		LOG("Incorrect state name: %s", state_str);
}
