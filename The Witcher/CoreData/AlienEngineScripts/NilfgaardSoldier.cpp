#include "NilfgaardSoldier.h"
#include "ArrowScript.h"
#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "EnemyManager.h"
#include "MusicController.h"

void NilfgaardSoldier::StartEnemy()
{
	type = EnemyType::NILFGAARD_SOLDIER;
	state = NilfgaardSoldierState::IDLE;
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

		stats["Health"] = Stat("Health", stat_weapon->GetNumber("Health"));
		stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("Agility"));
		stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("Damage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("AttackSpeed"));
		stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
		stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));

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

float NilfgaardSoldier::GetDamaged(float dmg, PlayerController* player, float3 knock)
{
	float damage = Enemy::GetDamaged(dmg, player, knock);

	if (can_get_interrupted || stats["Health"].GetValue() == 0.0F) {
		state = NilfgaardSoldierState::HIT;
		animator->PlayState("Hit");
		audio_emitter->StartSound("SoldierHit");
	}
	//else
	//{
	//	//Quizas que haga sonidito de ataque pero le han hecho pupita
	//}

	SpawnParticle("hit_particle", particle_spawn_positions[1]->transform->GetLocalPosition()); //1 is body position

	character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

	if (stats["Health"].GetValue() == 0.0F) {

		animator->SetBool("dead", true);
		OnDeathHit();

		if (player->attacks->GetCurrentAttack() && player->attacks->GetCurrentAttack()->IsLast())
		{
			state = NilfgaardSoldierState::DYING;
			audio_emitter->StartSound("SoldierDeath");

			float3 head_pos = transform->GetGlobalPosition();
			head_pos.y += 1.0f;

			decapitated_head = GameObject::Instantiate(head_prefab, head_pos);
			if (decapitated_head)
			{
				game_object->GetChild("Head")->SetEnable(false); //disable old head
				SpawnParticle("decapitation_particle", particle_spawn_positions[0]->transform->GetLocalPosition()); //0 is head position

				ComponentRigidBody* head_rb = decapitated_head->GetComponent<ComponentRigidBody>();
				head_rb->SetRotation(transform->GetGlobalRotation());

				float decapitation_force = 2.0f;
				float3 decapitation_vector = ((transform->GetGlobalPosition() - player->transform->GetGlobalPosition()).Normalized()) * decapitation_force * 0.5f;
				decapitation_vector += transform->up * decapitation_force;
	
				head_rb->AddForce(decapitation_vector);
				head_rb->AddTorque(decapitated_head->transform->up * decapitation_force);
				head_rb->AddTorque(decapitated_head->transform->forward * decapitation_force * 0.5f);
			}

			player->OnEnemyKill();
		}
	}

	return damage;
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
	if ((distance < stats["AttackRange"].GetValue()))
	{
		animator->SetFloat("speed", 0.0F);
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		Action();
	}

	if (distance > stats["VisionRange"].GetValue())
	{
		state = NilfgaardSoldierState::IDLE;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		is_combat = false;
	}
}

void NilfgaardSoldier::RotateSoldier()
{
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

void NilfgaardSoldier::CleanUpEnemy()
{
	if (decapitated_head)
	{
		for (auto it = particles.begin(); it != particles.end(); ++it)
		{
			ReleaseParticle((*it)->GetName());
		}

		decapitated_head->ToDelete();
		decapitated_head = nullptr;
	}
}

void NilfgaardSoldier::Stun(float time)
{
	if (state != NilfgaardSoldierState::STUNNED)
	{
		state = NilfgaardSoldierState::STUNNED;
		animator->PlayState("Dizzy");
		current_stun_time = Time::GetGameTime();
		stun_time = time;
	}
}

bool NilfgaardSoldier::IsDead()
{
	return (state == NilfgaardSoldierState::DEAD ? true : false);
}

void NilfgaardSoldier::SetState(const char* state_str)
{
	if (state_str == "Idle")
		state = NilfgaardSoldierState::IDLE;
	else if (state_str == "Move")
		state = NilfgaardSoldierState::MOVE;
	else if (state_str == "Attack")
		state = NilfgaardSoldierState::ATTACK;
	else if (state_str == "Block" || state_str == "Flee")
		state = NilfgaardSoldierState::AUXILIAR;
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
		can_get_interrupted = true;
		ReleaseParticle("EnemyAttackParticle");
		if (distance < stats["VisionRange"].GetValue())
		{
			state = NilfgaardSoldierState::MOVE;
		}
		else
		{
			state = NilfgaardSoldierState::IDLE;
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		}
	}
	else if (strcmp(name, "Hit") == 0) {
		ReleaseParticle("hit_particle");
		if (stats["Health"].GetValue() == 0.0F) {
			state = NilfgaardSoldierState::HIT;
		}
		else
		{
			state = NilfgaardSoldierState::IDLE;
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		}
	}
	else if ((strcmp(name, "Dizzy") == 0) && stats["Health"].GetValue() <= 0)
	{
		state = NilfgaardSoldierState::DYING;
		GameManager::instance->player_manager->IncreaseUltimateCharge(10);
	}
}

void NilfgaardSoldier::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != NilfgaardSoldierState::DEAD) {
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player && player->attacks->GetCurrentAttack()->CanHit(this))
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			float3 knock = (this->transform->GetGlobalPosition() - player->game_object->transform->GetGlobalPosition()).Normalized();
			knock = knock * player->attacks->GetCurrentAttack()->info.stats["KnockBack"].GetValue();

			player->OnHit(this, GetDamaged(dmg_received, player, knock));

			if (state == NilfgaardSoldierState::DYING)
				player->OnEnemyKill();

			HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
		}
	}
}
