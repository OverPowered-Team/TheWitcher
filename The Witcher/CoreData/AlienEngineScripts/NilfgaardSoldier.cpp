#include "NilfgaardSoldier.h"
#include "ArrowScript.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "EnemyManager.h"
#include "MusicController.h"

void NilfgaardSoldier::StartEnemy()
{
	type = EnemyType::NILFGAARD_SOLDIER;
	m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
	Enemy::StartEnemy();

}

void NilfgaardSoldier::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
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

		stats["Health"] = Stat("Health", stat_weapon->GetNumber("Health"));
		stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("Agility"));
		stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("Damage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("AttackSpeed"));
		stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
		if (nilf_type == NilfgaardType::ARCHER)
		{
			stats["FleeRange"] = Stat("FleeRange", stat_weapon->GetNumber("FleeRange"));
			stats["FleeRange"].SetMaxValue(stat_weapon->GetNumber("MaxFleeRange"));
		}
		else if (nilf_type == NilfgaardType::SWORD_SHIELD)
		{
			stats["BlockRange"] = Stat("BlockRange", stat_weapon->GetNumber("BlockRange"));
		}
		stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));

		stat_weapon->GetAnotherNode();
	}

	JSONfilepack::FreeJSON(stat);
}

void NilfgaardSoldier::Action()
{
	switch (nilf_type)
	{
	case NilfgaardSoldier::NilfgaardType::SWORD:
		animator->PlayState("Attack");
		state = EnemyState::ATTACK;
		break;
	case NilfgaardSoldier::NilfgaardType::ARCHER:
		animator->PlayState("Shoot");
		state = EnemyState::ATTACK;
		break;
	case NilfgaardSoldier::NilfgaardType::SWORD_SHIELD:
		int rand_num = Random::GetRandomIntBetweenTwo(0, 1);
		if (rand_num == 0)
		{
			animator->PlayState("Block");
			current_time = Time::GetGameTime();
			is_blocked = true;
			state = EnemyState::BLOCK;
		}
		else
		{
			animator->PlayState("Attack");
			state = EnemyState::ATTACK;
		}
		break;
	}
}

void NilfgaardSoldier::Block()
{
	float b_time = (has_been_attacked) ? block_attack_time : block_time;
	if (Time::GetGameTime() - current_time > b_time)
	{
		if (stats["AttackRange"].GetValue() < distance)
		{
			state = EnemyState::IDLE;
			animator->PlayState("Idle");
		}
		else
		{
			state = EnemyState::ATTACK;
			animator->PlayState("Attack");
		}

		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
	else if (break_shield_attack >= max_break_shield_attack)
	{
		state = EnemyState::ATTACK;
		animator->PlayState("Hit");
		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
}

void NilfgaardSoldier::Flee(float3 direction)
{
	float3 velocity_vec = direction * stats["Agility"].GetValue();
	character_ctrl->Move(velocity_vec);
	animator->SetFloat("speed", stats["Agility"].GetValue());

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

	if (distance > stats["FleeRange"].GetMaxValue())
	{
		state = Enemy::EnemyState::ATTACK;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		Action();
	}
}

void NilfgaardSoldier::ShootAttack()
{
	float3 arrow_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(arrow, arrow_pos);
	ComponentRigidBody* arrow_rb = arrow_go->GetComponent<ComponentRigidBody>();
	audio_emitter->StartSound("SoldierShoot");
	arrow_go->GetComponent<ArrowScript>()->damage = stats["Damage"].GetValue();
	arrow_rb->SetRotation(RotateArrow());
	arrow_rb->AddForce(direction.Mul(20), ForceMode::IMPULSE);
}

Quat NilfgaardSoldier::RotateArrow()
{
	float3 front = -float3::unitZ(); //front of the object
	Quat rot1 = Quat::RotateFromTo(front, direction);

	float3 desiredUp = float3::unitY();
	float3 right = Cross(direction, desiredUp);
	desiredUp = Cross(right, direction);

	float3 newUp = rot1 * float3(0.0f, 1.0f, 0.0f);
	Quat rot2 = Quat::RotateFromTo(newUp, desiredUp);
	return rot2 * rot1;
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

void NilfgaardSoldier::UpdateEnemy()
{	
	Enemy::UpdateEnemy();

	switch (state)
	{
	case Enemy::EnemyState::IDLE:
		if (distance < stats["VisionRange"].GetValue()) {
			state = Enemy::EnemyState::MOVE;
			m_controller->is_combat = true; //Note: This should be placed to every enemy type and not especifically in each enemy
			m_controller->has_changed = true;
		}
		else if (nilf_type == NilfgaardType::ARCHER && distance < stats["FleeRange"].GetValue())
			state = Enemy::EnemyState::FLEE;
		break;
	case Enemy::EnemyState::MOVE:
		if (distance > stats["VisionRange"].GetValue())
		{
			m_controller->is_combat = false;
			m_controller->has_changed = true;
		}
		Move(direction);
		break;
	case Enemy::EnemyState::ATTACK:
		switch (nilf_type)
		{
		case NilfgaardSoldier::NilfgaardType::ARCHER:
			float angle = atan2f(direction.z, direction.x);
			Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
			transform->SetGlobalRotation(rot);

			if (distance < stats["FleeRange"].GetValue())
			{
				animator->PlayState("Walk");
				state = Enemy::EnemyState::FLEE;
			}
			break;
		}
		break;
	case Enemy::EnemyState::BLOCK:
		if (stats["BlockRange"].GetValue() < distance)
		{
			state = EnemyState::IDLE;
			animator->PlayState("Idle");
		}
		Block();
		break;
	case Enemy::EnemyState::FLEE:
		Flee(-direction);
		break;
	case Enemy::EnemyState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		//Ori Ori function sintaxis
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		audio_emitter->StartSound("SoldierDeath");
		state = EnemyState::DEAD;
		m_controller->is_combat = false;
		m_controller->has_changed = true;
		break;
	}
	case Enemy::EnemyState::DEAD:
		break;
	}

}

void NilfgaardSoldier::OnAnimationEnd(const char* name) {

	if (strcmp(name, "Attack") == 0 || strcmp(name, "Shoot") == 0) {
		if (distance < stats["VisionRange"].GetValue())
		{
			state = Enemy::EnemyState::MOVE;
			
		}
		else
		{
			state = Enemy::EnemyState::IDLE;
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
			m_controller->is_combat = false;
			m_controller->has_changed = true;
		}
	}

	if (strcmp(name, "Hit") == 0) {
		if (stats["Health"].GetValue() == 0.0F) {
			state = EnemyState::HIT;
		}
		else
		{
			state = Enemy::EnemyState::IDLE;
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		}
	}

	if (strcmp(name, "Dizzy") == 0)
	{
		state = EnemyState::DYING;
		GameManager::instance->player_manager->IncreaseUltimateCharge(10);
		//need to know last enemy who hit him to count kill?
	}
}

void NilfgaardSoldier::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != EnemyState::DEAD) {

		if (is_blocked)
		{
			has_been_attacked = true;
			current_time = Time::GetGameTime();
			break_shield_attack++;
			LOG("breakshield: %i", break_shield_attack);
			particles["ClinckEmitter"]->Restart();
			audio_emitter->StartSound("SoldierBlock");
		}
		else
		{
			PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
			if (player)
			{
				float dmg_received = player->attacks->GetCurrentDMG();
				player->OnHit(this, GetDamaged(dmg_received, player));
				LOG("live: %f", stats["Health"].GetValue());

				if (state == EnemyState::DYING)
					player->OnEnemyKill();

				HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
			}
		}
	}
}
