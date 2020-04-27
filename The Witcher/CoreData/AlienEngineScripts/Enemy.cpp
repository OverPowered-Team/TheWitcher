#include "Enemy.h"
#include "EnemyManager.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"

void Enemy::Awake()
{
	GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>()->AddEnemy(this);
	attack_collider = game_object->GetChild("EnemyAttack")->GetComponent<ComponentCollider>();
	attack_collider->SetEnable(false);
}

void Enemy::StartEnemy()
{
	animator = GetComponent<ComponentAnimator>();
	character_ctrl = GetComponent<ComponentCharacterController>();
	audio_emitter = GetComponent<ComponentAudioEmitter>();
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
	case EnemyType::LESHEN:
		json_str = "leshen";
		break;
	default:
		break;
	}

	SetStats(json_str.data());

	
	if (game_object->GetChild("Particles"))
	{
		std::vector<ComponentParticleSystem*> particle_gos = game_object->GetChild("Particles")->GetComponentsInChildren<ComponentParticleSystem>();

		for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
			particles.insert(std::pair((*it)->game_object_attached->GetName(), (*it)));
			(*it)->OnStop();
		}
	}
}

void Enemy::UpdateEnemy()
{
	float distance_1 = player_controllers[0]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_controllers[0]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_controllers[1]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_controllers[1]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	if (player_controllers[0]->state->type == StateType::DEAD)
	{
		distance = distance_2;
		direction = direction_2.Normalized();
	}
	else if (player_controllers[1]->state->type == StateType::DEAD)
	{
		distance = distance_1;
		direction = direction_1.Normalized();
	}
	else
	{
		distance = (distance_1 < distance_2) ? distance_1 : distance_2;
		direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();
	}

	for (auto it = effects.begin(); it != effects.end(); )
	{
		if ((*it)->UpdateEffect() && (*it)->ticks_time > 0)
		{
			for (auto it_stats = stats.begin(); it_stats != stats.end(); ++it_stats)
			{
				it_stats->second.ModifyCurrentStat((*it));

				//Temporal solution
				if (it_stats->first == "Health")
				{
					if (stats["Health"].GetValue() == 0)
					{
						state = EnemyState::DYING;
						animator->PlayState("Death");
					}
				}
			}
			if (particles["p_" + (*it)->name])
				particles["p_" + (*it)->name]->Restart();
				
		}
		if ((*it)->to_delete)
		{
			delete (*it);
			it = effects.erase(it);
			continue;
		}
		++it;
	}

}

void Enemy::CleanUpEnemy()
{
	if (decapitated_head)
	{
		decapitated_head->ToDelete();
		decapitated_head = nullptr;
	}
}

void Enemy::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
	if (stat)
	{
		stats["Health"] = Stat( "Health", stat->GetNumber("Health"));
		stats["Agility"] = Stat("Agility", stat->GetNumber("Agility"));
		stats["Damage"] = Stat("Damage", stat->GetNumber("Damage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat->GetNumber("AttackSpeed"));
		stats["AttackRange"] = Stat("AttackRange", stat->GetNumber("AttackRange"));
		stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
	}

	JSONfilepack::FreeJSON(stat);
}

void Enemy::Move(float3 direction)
{
	float3 velocity_vec = direction * stats["Agility"].GetValue();
	character_ctrl->Move(velocity_vec * Time::GetScaleTime() * Time::GetDT());
	animator->SetFloat("speed", stats["Agility"].GetValue());

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

	if ((distance < stats["AttackRange"].GetValue() && type != EnemyType::GHOUL) || (type == EnemyType::GHOUL && distance < stats["JumpRange"].GetValue()))
	{
		animator->SetFloat("speed", 0.0F);
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		Action();
	}

	if (distance > stats["VisionRange"].GetValue())
	{
		state = Enemy::EnemyState::IDLE;
		character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
		animator->SetFloat("speed", 0.0F);
		is_combat = false;
	}
}

void Enemy::ActivateCollider()
{
	if (attack_collider)
	{
		attack_collider->SetEnable(true);
	}
}

void Enemy::DeactivateCollider()
{
	if (attack_collider)
	{
		attack_collider->SetEnable(false);
	}
}

void Enemy::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != EnemyState::DEAD) {
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player)
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			player->OnHit(this, GetDamaged(dmg_received, player));
		}
	}
}

float Enemy::GetDamaged(float dmg, PlayerController* player)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);
	
	if (can_get_interrupted) {
		state = EnemyState::HIT;
		animator->PlayState("Hit");
	}

	switch (type)
	{
	case EnemyType::GHOUL:
		audio_emitter->StartSound("GhoulHit");
		break;
	case EnemyType::NILFGAARD_SOLDIER:
		audio_emitter->StartSound("SoldierHit");
		if(particles["hit_particle"])
			particles["hit_particle"]->Restart();
		break;
	}

	character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

	if (stats["Health"].GetValue() == 0.0F) {

		animator->SetBool("dead", true);
		OnDeathHit();

		if (type == EnemyType::LESHEN) {
			state = EnemyState::DYING;
			animator->PlayState("Death");
		}
		else if (player->attacks->GetCurrentAttack()->IsLast())
		{
			state = EnemyState::DYING;
			animator->PlayState("Death");

			switch (type)
			{
			case EnemyType::GHOUL:
				audio_emitter->StartSound("GhoulDeath");
				break;
			case EnemyType::NILFGAARD_SOLDIER:
				audio_emitter->StartSound("SoldierDeath");
				break;
			}

			float3 head_pos = transform->GetGlobalPosition();
			head_pos.y += 1.0f;

			decapitated_head = GameObject::Instantiate(head_prefab, head_pos);
			if (decapitated_head)
			{
				game_object->GetChild("Head")->SetEnable(false); //disable old head
				particles["decapitation_particle"]->Restart();

				ComponentRigidBody* head_rb = decapitated_head->GetComponent<ComponentRigidBody>();
				head_rb->SetRotation(transform->GetGlobalRotation());
				//head_rb->SetPosition(head_position->transform->GetGlobalPosition());

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

	return aux_health - stats["Health"].GetValue();
}

void Enemy::AddEffect(Effect* new_effect)
{
	effects.push_back(new_effect);

	for (auto it = stats.begin(); it != stats.end(); ++it)
	{
		if (new_effect->AffectsStat(it->second.name) && new_effect->ticks_time == 0)
			it->second.ApplyEffect(new_effect);
	}
}

void Enemy::HitFreeze(float freeze_time)
{
	if (!is_frozen)
	{
		is_frozen = true;
		float speed = animator->GetCurrentStateSpeed();
		animator->SetCurrentStateSpeed(0);
		ComponentAnimator* anim = animator;
		Invoke([this, speed]() -> void {Enemy::StopHitFreeze(speed); }, freeze_time);
	}
}

void Enemy::StopHitFreeze(float speed)
{
	is_frozen = false;
	animator->SetCurrentStateSpeed(speed);
}
