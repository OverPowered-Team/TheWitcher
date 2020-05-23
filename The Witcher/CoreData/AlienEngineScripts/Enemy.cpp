#include "Enemy.h"
#include "EnemyManager.h"
#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"

void Enemy::Awake()
{
	GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>()->AddEnemy(this);
	attack_collider = game_object->GetChild("EnemyAttack")->GetComponent<ComponentCollider>();
	attack_collider->SetEnable(false);

	//0.Head 1.Body 2.Feet 3.Attack
	particle_spawn_positions = game_object->GetChild("Particle_Positions")->GetChildren();
}

void Enemy::StartEnemy()
{
	animator = GetComponent<ComponentAnimator>();
	character_ctrl = GetComponent<ComponentCharacterController>();
	audio_emitter = GetComponent<ComponentAudioEmitter>();
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
	case EnemyType::CIRI:
		json_str = "ciri";
		break;
	case EnemyType::CIRI_CLONE:
		json_str = "ciri_clone";
		break;
	case EnemyType::DROWNED:
		json_str = "drowned";
		break;
	case EnemyType::BLOCKER_OBSTACLE:
		json_str = "blockerobstacle";
		break;
	default:
		break;
	}

	SetStats(json_str.data());

	start_pos = game_object->GetComponent<ComponentTransform>()->GetGlobalPosition(); 

	/*if (game_object->GetChild("Particles"))
	{
		std::vector<ComponentParticleSystem*> particle_gos = game_object->GetChild("Particles")->GetComponentsInChildren<ComponentParticleSystem>();

		for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
			particles.insert(std::pair((*it)->game_object_attached->GetName(), (*it)));
			(*it)->OnStop();
		}
	}*/
}

void Enemy::UpdateEnemy()
{
	float distance_1 = player_controllers[0]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_controllers[0]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_controllers[1]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_controllers[1]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	if (player_controllers[0]->state->type == StateType::DEAD)
	{
		current_player = 1;
		distance = distance_2;
		direction = direction_2.Normalized();
	}
	else if (player_controllers[1]->state->type == StateType::DEAD)
	{
		current_player = 0;
		distance = distance_1;
		direction = direction_1.Normalized();
	}
	else
	{
		distance = (distance_1 < distance_2) ? distance_1 : distance_2;
		direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();
		current_player = (distance_1 < distance_2) ? 0 : 1;
	}

	character_ctrl->Move(float3::unitY() * gravity * Time::GetDT());

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
						SetState("Dying");
					}
				}
			}
			if ((*it)->spawned_particle != nullptr)
			{
				(*it)->spawned_particle->SetEnable(false);
				(*it)->spawned_particle->SetEnable(true);
			}

			std::string audio_name = "Play_" + (*it)->name;
			audio_emitter->StartSound(audio_name.c_str());
		}

		if ((*it)->to_delete)
		{
			if ((*it)->spawned_particle != nullptr)
			{
				GameManager::instance->particle_pool->ReleaseInstance((*it)->vfx_on_apply, (*it)->spawned_particle);
			}
			for (auto stat_it = stats.begin(); stat_it != stats.end(); ++stat_it)
			{
				if ((*it)->AffectsStat(stat_it->second.name))
					stat_it->second.RemoveEffect((*it));
			}
			delete (*it);
			it = effects.erase(it);
			continue;
		}
		++it;
	}

}

void Enemy::CleanUpEnemy()
{
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
		stats["KnockBack"] = Stat("KnockBack", stat->GetNumber("KnockBack"));
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

	CheckDistance();
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

Quat Enemy::RotateProjectile()
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

float Enemy::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	last_player_hit = player;
	velocity = knock_back; //This will replace old knockback if there was any...

	return aux_health - stats["Health"].GetValue();
}

float Enemy::GetDamaged(float dmg, float3 knock_back)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	velocity = knock_back;

	return aux_health - stats["Health"].GetValue();
}

void Enemy::AddEffect(Effect* new_effect)
{
	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		if ((*it)->name == new_effect->name)
		{
			(*it)->start_time = Time::GetGameTime(); //Refresh timer
			delete new_effect;
			return;
		}
	}

	effects.push_back(new_effect);

	if (new_effect->vfx_on_apply != "")
		new_effect->spawned_particle = GameManager::instance->particle_pool->GetInstance(new_effect->vfx_on_apply,
			particle_spawn_positions[new_effect->vfx_position]->transform->GetLocalPosition(), float3::zero(), this->game_object, true);

	if (new_effect->ticks_time == 0)
	{
		std::string audio_name = "Play_" + new_effect->name;
		audio_emitter->StartSound(audio_name.c_str());
	}

	for (auto it = stats.begin(); it != stats.end(); ++it)
	{
		if (new_effect->AffectsStat(it->second.name) && new_effect->ticks_time == 0)
			it->second.ApplyEffect(new_effect);
	}
}

void Enemy::RemoveEffect(Effect* _effect)
{
	for (auto it = stats.begin(); it != stats.end(); ++it)
	{
		if (_effect->AffectsStat(it->second.name))
			it->second.RemoveEffect(_effect);
	}

	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		if ((*it) == _effect)
		{
			delete _effect;
			effects.erase(it);
			return;
		}
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

void Enemy::SpawnAttackParticle()
{
	SpawnParticle("EnemyAttackParticle", particle_spawn_positions[3]->transform->GetLocalPosition());
	HitFreeze(0.05);
	can_get_interrupted = false;
	// Sonidito de clinck de iluminacion espada maestra
}


void Enemy::StopHitFreeze(float speed)
{
	is_frozen = false;
	animator->SetCurrentStateSpeed(speed);
}

void Enemy::SpawnParticle(std::string particle_name, float3 pos, bool local, float3 rotation, GameObject* parent)
{
	if (particle_name == "")
	{
		LOG("There's no particle name. String is empty!");
		return;
	}

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), particle_name.c_str()) == 0)
		{
			(*it)->SetEnable(false);
			(*it)->SetEnable(true);
			return;
		}
	}

	parent = parent != nullptr ? parent : this->game_object;
	rotation = rotation.IsZero() ? parent->transform->GetGlobalRotation().ToEulerXYZ() : rotation;
	GameObject* new_particle = GameManager::instance->particle_pool->GetInstance(particle_name, pos, rotation, parent, local);
	if (new_particle == nullptr)
		return;

	particles.push_back(new_particle);
}

void Enemy::ReleaseParticle(std::string particle_name)
{
	if (particle_name == "")
		return;

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), particle_name.c_str()) == 0)
		{
			GameManager::instance->particle_pool->ReleaseInstance(particle_name, (*it));
			particles.erase(it);
			return;
		}
	}
}

void Enemy::ReleaseAllParticles()
{
	for (auto it = particles.begin(); it != particles.end();)
	{
		GameManager::instance->particle_pool->ReleaseInstance((*it)->GetName(), (*it));
		it = particles.erase(it);
	}
}
	
