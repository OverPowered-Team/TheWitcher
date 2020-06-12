#include "Enemy.h"
#include "EnemyManager.h"
#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"
#include "SteeringAvoid.h"
#include "UI_DamageCount.h"

void Enemy::Awake()
{
	GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>()->AddEnemy(this);
	attack_collider = game_object->GetChild("EnemyAttack")->GetComponent<ComponentCollider>();
	attack_collider->SetEnable(false);

	//0.Head 1.Body 2.Feet 3.Attack
	particle_spawn_positions.push_back(game_object->GetChildRecursive("Head_Position"));
	particle_spawn_positions.push_back(game_object->GetChildRecursive("Body_Position"));
	particle_spawn_positions.push_back(game_object->GetChildRecursive("Feet_Position"));
	particle_spawn_positions.push_back(game_object->GetChildRecursive("Attack_Position"));

	hitMaterial.color = float4(1, 1, 1, 1);

	GameObject* mesh = game_object->GetChild("Mesh");
	if (mesh)
	{
		meshes = mesh->GetComponentsInChildrenRecursive<ComponentMaterial>();
		//for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
		//{
		//	if ((*iter) && (*iter)->HasChildren())
		//	{
		//		std::vector<GameObject*> childs = (*iter)->GetChildren();
		//		for (auto iter2 = childs.begin(); iter2 != meshes.end(); iter2++)
		//		{
		//			if ((*iter2))
		//			{
		//				meshes.push_back((*iter2));
		//			}
		//		}	
		//	}
		//}
		LOG("Got Meshes");
	}

}

void Enemy::StartEnemy()
{
	animator = GetComponent<ComponentAnimator>();
	character_ctrl = GetComponent<ComponentCharacterController>();
	audio_emitter = GetComponent<ComponentAudioEmitter>();
	steeringAvoid = GetComponent<SteeringAvoid>();

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

	is_hit_inmune = GameManager::instance->enemy_manager->is_hit_inmune;

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
	float distance_1 = player_controllers[0]->transform->GetGlobalPosition().Distance(game_object->transform->GetGlobalPosition());
	float3 direction_1 = player_controllers[0]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_controllers[1]->transform->GetGlobalPosition().Distance(game_object->transform->GetGlobalPosition());
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
		if (!is_battle_circle)
		{
			/*if (player_controllers[0]->enemy_battle_circle.size() == player_controllers[1]->enemy_battle_circle.size())
			{*/
			distance = (distance_1 < distance_2) ? distance_1 : distance_2;
			direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();
			current_player = (distance_1 < distance_2) ? 0 : 1;
			/*}
			else if (player_controllers[0]->enemy_battle_circle.size() < player_controllers[1]->enemy_battle_circle.size())
			{
				distance = distance_1;
				direction = direction_1;
				current_player = 0;
			}
			else if (player_controllers[1]->enemy_battle_circle.size() < player_controllers[0]->enemy_battle_circle.size())
			{
				distance = distance_2;
				direction = direction_2;
				current_player = 1;
			}*/
		}
		else
		{
			if (current_player == 0 && distance_1 > distance_2 + 10)
			{
				RemoveBattleCircle();
				current_player = 1;
			}
			else if (current_player == 1 && distance_2 > distance_1 + 10)
			{
				RemoveBattleCircle();
				current_player = 0;
			}

			distance = (current_player == 0) ? distance_1 : distance_2;
			direction = (current_player == 0) ? direction_1 : direction_2;
		}
	}

	if(type != EnemyType::BLOCKER_OBSTACLE)
		character_ctrl->Move(float3::unitY() * gravity * Time::GetDT());

	if(type != EnemyType::BLOCKER_OBSTACLE)
		animator->SetBool("grounded", character_ctrl->isGrounded);

	for (auto it = effects.begin(); it != effects.end(); )
	{
		if ((*it)->UpdateEffect() && (*it)->ticks_time > 0)
		{
			for (auto it_stats = stats.begin(); it_stats != stats.end(); ++it_stats)
			{
				it_stats->second.ModifyCurrentStat((*it));
				
				//Temporal solution
				if (it_stats->first == "Health" && !IsDead() && !IsDying())
				{
					if (stats["Health"].GetValue() <= 0)
					{
						SetState("Dying");
						PlaySFX("Death");
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
	if (inHit)
	{
		whiteTime += Time::GetDT();
		if (whiteTime > 0.1)
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				if ((*iter))
				{
					/*ComponentMaterial* material = (*iter)->GetComponent<ComponentMaterial>();
					if (material)
					{*/
						if(defaultMaterial)
							(*iter)->material = defaultMaterial;
					/*}*/
				}
			}
			whiteTime = 0;
			inHit = false;
		}
	}
}

void Enemy::CleanUpEnemy()
{
	ReleaseAllParticles();
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
		stats["KnockBack"] = Stat("KnockBack", stat->GetNumber("KnockBack"));
	}

	JSONfilepack::FreeJSON(stat);
}

void Enemy::Move(float3 direction)
{
	
	float avoid_force = 0.0f;
	float3 velocity_vec = direction.Normalized() * stats["Acceleration"].GetValue() * Time::GetDT();
	float3 avoid_vector = steeringAvoid->AvoidObstacle(avoid_force);

	// Option 1 : Works only one steering at the time. Avoid zone has to be small
	/*if (avoid_vector.Length() > 0)
		velocity += avoid_vector;
	else
		velocity += velocity_vec;*/

	// Option 2: Add of the two steering
	//velocity += avoid_vector + velocity_vec;

	// Option 3: Add of two steering with weights
	velocity += avoid_vector * avoid_force + velocity_vec * (1 - avoid_force);

	if (velocity.Length() > stats["Agility"].GetValue())
	{
		velocity = velocity.Normalized()* stats["Agility"].GetValue();
	}
	LOG("%f", velocity.Length());
	character_ctrl->Move(velocity * Time::GetDT() * Time::GetScaleTime());
	animator->SetFloat("speed", velocity.Length());

	float angle = atan2f(velocity.z, velocity.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

	CheckDistance();
}

void Enemy::Guard()
{
	float avoid_force = 0.0f;
	float3 velocity_vec = direction.Normalized() * stats["Acceleration"].GetValue() * Time::GetDT();
	float3 avoid_vector = steeringAvoid->AvoidObstacle(avoid_force);

	// Option 1: Combiantion of behaviours
	/*if (player_controllers[current_player]->battleCircle < distance)
		velocity += avoid_vector * avoid_force + velocity_vec * (1 - avoid_force);
	else
		velocity = float3::zero();*/

	// Option 2: Different behaviours
	if (avoid_vector.LengthSq() > 0)
		velocity += avoid_vector;
	else if (player_controllers[current_player]->battleCircle < distance && stats["AttackRange"].GetValue() < distance)
		velocity += velocity_vec;
	else if (stats["AttackRange"].GetValue() > distance)
		velocity -= velocity_vec;
	else 
		velocity = float3::zero();

	animator->SetFloat("speed", velocity.Length());

	if (velocity.Length() > stats["Agility"].GetValue())
		velocity = velocity.Normalized() * stats["Agility"].GetValue();

	character_ctrl->Move(velocity * Time::GetDT() * Time::GetScaleTime());

	float angle;
	if(velocity.Length() == 0.0f)
		angle = atan2f(velocity_vec.z, velocity_vec.x);
	else
		angle = atan2f(velocity.z, velocity.x);

	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);

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

void Enemy::Decapitate(PlayerController* player)
{
	decapitated_head = GameObject::Instantiate(head_prefab, particle_spawn_positions[0]->transform->GetGlobalPosition());

	if (decapitated_head)
	{
		// If not working, check prefab
		game_object->GetChild("Mesh")->GetChild("Head")->SetEnable(false); //disable old head
		SpawnParticle(decapitation_particle, particle_spawn_positions[0]->transform->GetGlobalPosition()); //0 is head position
		
		vector<GameObject*> iss = decapitated_head->GetChildren();
		for (auto it = iss.begin(); it != iss.end(); it++)
		{
			if (strcmp((*it)->GetName(),"blood_head_particle" )==0)
			{
				(*it)->SetEnable(true);
			}
		}
		

		ComponentRigidBody* head_rb = decapitated_head->GetComponent<ComponentRigidBody>();
		head_rb->SetRotation(particle_spawn_positions[0]->transform->GetGlobalRotation());

		float decapitation_force = 2.0f;
		float3 decapitation_vector = ((transform->GetGlobalPosition() - player->transform->GetGlobalPosition()).Normalized()) * decapitation_force * 0.5f;
		decapitation_vector += transform->up * decapitation_force;

		head_rb->AddForce(decapitation_vector);
		head_rb->AddTorque(decapitated_head->transform->up * decapitation_force);
		head_rb->AddTorque(decapitated_head->transform->forward * decapitation_force * 0.5f);
	}
}

void Enemy::CanGetInterrupted()
{
	//can_get_interrupted = true;
}

void Enemy::RotatePlayer()
{
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

void Enemy::RotateToPlayerSmooth(float perc)
{
	float angle = atan2f(direction.z, direction.x) * perc;
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

float Enemy::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	SetState("Hit");

	if (GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>())
	{
		GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>()->AddDamageCount(dmg, player);
	}

	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	last_player_hit = player;
	velocity = knock_back; //This will replace old knockback if there was any...

	if (can_get_interrupted)
	{
		animator->PlayState("Hit");
		PlaySFX("Hit");
	}

	float2 temp_e = float2(particle_spawn_positions[1]->transform->GetGlobalPosition().x, particle_spawn_positions[1]->transform->GetGlobalPosition().z);
	float2 temp_pl = float2(last_player_hit->transform->GetGlobalPosition().x, last_player_hit->transform->GetGlobalPosition().z);
	float angle = acos(math::Dot(temp_e, temp_pl) / (temp_e.LengthSq() * temp_pl.LengthSq())); //sino provar Length	
	LOG("%f", angle);//radians
	float anglez = math::RadToDeg(angle);
	LOG("%f", anglez);//degree
	float3 rotated_particle = float3(
		temp_e.x * cos(anglez) - temp_pl.y * sin(anglez),
		0.0f,
		temp_e.x * sin(anglez) + temp_pl.y * cos(anglez));
	LOG("X %f, Z %f", rotated_particle.x, rotated_particle.z);
	math::Quat player_quat =last_player_hit->transform->GetGlobalRotation();
	float3 particle_rotation = last_player_hit->attacks->GetCurrentAttack()->info.hit_particle_dir;

	SpawnParticle(last_player_hit->attacks->GetCurrentAttack()->info.hit_particle_name, particle_spawn_positions[1]->transform->GetGlobalPosition(),false, particle_rotation,nullptr,player_quat);
	character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

	if (stats["Health"].GetValue() <= 0.0F) {

		animator->SetBool("dead", true);
		is_dead = true;
		OnDeathHit();

		if (player->attacks->GetCurrentAttack() && player->attacks->GetCurrentAttack()->IsLast())
		{
			SetState("Dying");
			PlaySFX("Death");

			Decapitate(player);
		}
	}

	HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
	for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
	{
		if ((*iter))
		{
			//ComponentMaterial* material = (*iter)->GetComponent<ComponentMaterial>();
			if (defaultMaterial == nullptr)
			{
				defaultMaterial = (ResourceMaterial*)(*iter)->GetMaterial();
			}

			if ((*iter))
			{
				(*iter)->material = &hitMaterial;
			}
		}
		inHit = true;
	}
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
			particle_spawn_positions[new_effect->vfx_position]->transform->GetGlobalPosition(), float3::zero(), this->game_object, false);

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
	if (_effect->spawned_particle != nullptr)
	{
		GameManager::instance->particle_pool->ReleaseInstance(_effect->vfx_on_apply, _effect->spawned_particle);
	}

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
		std::string state_freezed = animator->GetCurrentStateName();
		animator->SetCurrentStateSpeed(0);
		ComponentAnimator* anim = animator;
		Invoke([this, speed, state_freezed]() -> void {Enemy::StopHitFreeze(speed, state_freezed); }, freeze_time);
	}
}

void Enemy::SpawnAttackParticle()
{
	/*SpawnParticle("EnemyAttackParticle", particle_spawn_positions[3]->transform->GetGlobalPosition());
	HitFreeze(0.05);
	can_get_interrupted = false;*/
	// Sonidito de clinck de iluminacion espada maestra
}


void Enemy::StopHitFreeze(float speed, std::string name)
{
	is_frozen = false;
	animator->SetStateSpeed(name.c_str(), speed);
}

void Enemy::SpawnParticle(std::string particle_name, float3 pos, bool local, float3 rotation, GameObject* parent,math::Quat quat_rot)
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
			
				//quat_rot.y = -quat_rot.y; 
			if(!quat_rot.Equals(math::Quat::identity()))
				(*it)->transform->SetGlobalRotation(quat_rot *quat_rot.RotateX(DEGTORAD * (rotation.x))* quat_rot.RotateY(DEGTORAD*(rotation.y))* quat_rot.RotateZ(DEGTORAD * (rotation.z)));
			
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
	
	if (!quat_rot.Equals(math::Quat::identity()))
		new_particle->transform->SetGlobalRotation(quat_rot * quat_rot.RotateX(DEGTORAD * (rotation.x)) * quat_rot.RotateY(DEGTORAD * (rotation.y)) * quat_rot.RotateZ(DEGTORAD * (rotation.z)));


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

void Enemy::ChangeAttackEnemy(bool deleting)
{
	RemoveAttacking(player_controllers[current_player]);

	std::vector<Enemy*> enemy_vec = player_controllers[current_player]->enemy_battle_circle;
	for (int i = 0; i < enemy_vec.size(); ++i)
	{
		if (!enemy_vec[i]->is_attacking && enemy_vec[i] != this && player_controllers[current_player]->current_attacking_enemies < player_controllers[current_player]->max_attacking_enemies && !enemy_vec[i]->IsDying() && !enemy_vec[i]->IsDead())
		{
			enemy_vec[i]->AddAttacking(player_controllers[current_player]);

			if (player_controllers[current_player]->current_attacking_enemies == player_controllers[current_player]->max_attacking_enemies)
				return;
		}
	}

	if (player_controllers[current_player]->current_attacking_enemies < player_controllers[current_player]->max_attacking_enemies && !deleting)
	{
		AddAttacking(player_controllers[current_player]);
	}
		
}

void Enemy::RemoveBattleCircle()
{
	for (auto it_enemy = player_controllers[current_player]->enemy_battle_circle.begin(); it_enemy != player_controllers[current_player]->enemy_battle_circle.end(); ++it_enemy)
	{
		if ((*it_enemy) == this)
		{
			ChangeAttackEnemy(true);
			is_battle_circle = false;
			player_controllers[current_player]->enemy_battle_circle.erase(it_enemy);
			return;
		}
	}
}

void Enemy::AddBattleCircle(PlayerController* player_controller)
{
	if (is_dead)
		return;

	is_battle_circle = true;
	player_controller->enemy_battle_circle.push_back(this);

	if (player_controller->current_attacking_enemies == player_controller->max_attacking_enemies)
	{
		SetState("Guard");
	}
	else
	{
		AddAttacking(player_controller);
	}
}

void Enemy::AddAttacking(PlayerController* player_controller)
{
	player_controller->current_attacking_enemies++;
	is_attacking = true;
	if(!is_dead)
		SetState("Move");
}

void Enemy::RemoveAttacking(PlayerController* player_controller)
{
	player_controllers[current_player]->current_attacking_enemies--;
	is_attacking = false;
	if(!is_dead)
		SetState("Guard");
}

void Enemy::OnControllerColliderHit(const ControllerColliderHit& hit)
{
	if (IsHit() && hit.collider->GetCollisionLayer() == "Player")
	{
		velocity = float3::zero();
	}
}

void Enemy::SpawnHealthOrb()
{
	int rand_num = Random::GetRandomIntBetweenTwo(0,2);
	if(rand_num == 0)
		GameObject::Instantiate(life_orb, transform->GetGlobalPosition() + float3::unitY() * 0.5);
}