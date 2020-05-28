#include "GameManager.h"
#include "RumblerManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "EffectsFactory.h"
#include "CameraShake.h"
#include "RumblerManager.h"
#include "ParticlePool.h"

#include "UI_Char_Frame.h"

#include "PlayerController.h"
#include "PlayerProjectile.h"

#include "Enemy.h"

#include "PlayerAttacks.h"

static std::unordered_map<std::string, Attack_Tags> const tag_table = { {"AOE",Attack_Tags::T_AOE}, {"Projectile",Attack_Tags::T_Projectile},
{"Trap",Attack_Tags::T_Trap}, {"Buff",Attack_Tags::T_Buff}, {"Debuff",Attack_Tags::T_Debuff}, {"Fire",Attack_Tags::T_Fire}, {"Ice",Attack_Tags::T_Ice},
{"Earth",Attack_Tags::T_Earth}, {"Lightning",Attack_Tags::T_Lightning}, {"Chaining",Attack_Tags::T_Chaining}, {"Spell",Attack_Tags::T_Spell} };

Attack_Tags GetTag(std::string str)
{
	if (auto it = tag_table.find(str); it != tag_table.end()) {
		return it->second;
	}
	return Attack_Tags::T_None;
}

static std::unordered_map<std::string, Collider_Type> const coll_table = { {"Box",Collider_Type::C_Box}, {"Sphere",Collider_Type::C_Sphere}, {"Weapon",Collider_Type::C_Weapon} };

Collider_Type GetColliderType(std::string str)
{
	if (auto it = coll_table.find(str); it != coll_table.end()) {
		return it->second;
	}
	return Collider_Type::C_Box;
}

PlayerAttacks::PlayerAttacks() : Alien()
{
}

PlayerAttacks::~PlayerAttacks()
{
}

void PlayerAttacks::Start()
{
	player_controller = GetComponent<PlayerController>();

	colliders = game_object->GetChild("Attacks_Collider")->GetComponents<ComponentCollider>();
	if (weapon_obj)
		colliders.push_back(weapon_obj->GetComponent<ComponentCollider>());

	for (std::vector<ComponentCollider*>::iterator it = colliders.begin(); it != colliders.end(); ++it)
	{
		(*it)->SetEnable(false);
	}
	
	shake = Camera::GetCurrentCamera()->game_object_attached->GetComponent<CameraShake>();

	CreateAttacks();
}

void PlayerAttacks::StartAttack(AttackType attack)
{
	SelectAttack(attack);
	DoAttack();
	AttackMovement();
}

bool PlayerAttacks::StartSpell(uint spell_index)
{
	if (spell_index < spells.size() && player_controller->player_data.stats["Chaos"].GetValue() >= spells[spell_index]->info.stats["Cost"].GetValue())
	{
		if (current_attack)
		{
			player_controller->ReleaseAttackParticle();
			//take the links of the attack we were doing so we can continue the combo after the spell.
			spells[spell_index]->heavy_attack_link = current_attack->heavy_attack_link;
			spells[spell_index]->light_attack_link = current_attack->light_attack_link;
		}
		else
		{
			spells[spell_index]->heavy_attack_link = nullptr;
			spells[spell_index]->light_attack_link = nullptr;
		}

		current_attack = spells[spell_index];
		DoAttack();
		AttackMovement();

		return true;
	}

	LOG("Not enough Chaos to cast Spell %i", spell_index);
	return false;
}

void PlayerAttacks::UpdateCurrentAttack()
{
	if (player_controller->animator->GetCurrentStateSpeed() == 0.0f) //THIS IS TO ADVANCE THE TIME THE ANIM HAS TO FINISH WHEN HITFREEZED
	{
		finish_attack_time += Time::GetDT();
		return;
	}

	if (current_target && Time::GetGameTime() < start_attack_time + snap_time)
		SnapToTarget();
	else
	{
		player_controller->player_data.velocity += player_controller->player_data.velocity * player_controller->player_data.slow_speed * Time::GetDT(); //SLOW DOWN
	}
		
	//IF ANIM FINISHED 
	if (Time::GetGameTime() >= finish_attack_time)
	{
		if (!player_controller->mov_input)
			player_controller->SetState(StateType::IDLE);
		else if (player_controller->mov_input)
			player_controller->SetState(StateType::RUNNING);
			
	}
	if (can_execute_input && next_attack != AttackType::NONE)
	{
		if (next_attack == AttackType::SPELL)
			StartSpell(next_spell);
		else
			StartAttack(next_attack);
		
		next_attack = AttackType::NONE;
	}
}

void PlayerAttacks::DoAttack()
{
	//Reset attack variables
	can_execute_input = false;
	next_attack = AttackType::NONE;
	current_target = nullptr;
	current_attack->current_collider = 0;
	distance_snapped = 0.0f;
	current_attack->enemies_hit.clear();

	player_controller->animator->PlayState(current_attack->info.name.c_str());

	//calculate end of attack
	start_attack_time = Time::GetGameTime();
	float animation_duration = player_controller->animator->GetCurrentStateDuration();
	finish_attack_time = start_attack_time + animation_duration;
}

void PlayerAttacks::SelectAttack(AttackType attack)
{
	if (!current_attack)
	{
		if(attack == AttackType::LIGHT)
			current_attack = base_light_attack;
		else
			current_attack = base_heavy_attack;
	}
	else
	{
		player_controller->ReleaseAttackParticle();
		if (attack == AttackType::LIGHT)
		{
			if (current_attack->light_attack_link)
				current_attack = current_attack->light_attack_link;
			else
				current_attack = base_light_attack;
		}
		else
		{
			if (current_attack->heavy_attack_link)
				current_attack = current_attack->heavy_attack_link;
			else
				current_attack = base_heavy_attack;
		}		
	}
}

std::vector<std::string> PlayerAttacks::GetFinalAttacks()
{
	std::vector<std::string> final_attacks;

	for (std::vector<Attack*>::iterator it = attacks.begin(); it != attacks.end(); ++it)
	{
		if ((*it)->IsLast())
			final_attacks.push_back((*it)->info.name);
	}
	return final_attacks;
}

void PlayerAttacks::OnAddAttackEffect(AttackEffect* new_effect)
{
	for (std::vector<Attack*>::iterator it = attacks.begin(); it != attacks.end(); ++it)
	{
		if ((*it)->info.name == new_effect->GetAttackIdentifier())
		{
			(*it)->info.stats["Damage"].ApplyEffect(new_effect);
		}
	}
}

void PlayerAttacks::OnRemoveAttackEffect(AttackEffect* new_effect)
{
}

void PlayerAttacks::CancelAttack()
{
	if (current_attack != nullptr)
	{
		player_controller->ReleaseAttackParticle();
		if(colliders[(int)current_attack->info.colliders[current_attack->current_collider].type])
			colliders[(int)current_attack->info.colliders[current_attack->current_collider].type]->SetEnable(false);
		
		current_attack = nullptr;
	}
}

void PlayerAttacks::SnapToTarget()
{
	float3 direction = (current_target->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	player_controller->transform->SetGlobalRotation(rot);

	float speed = 0.0f;
	float distance = abs(transform->GetGlobalPosition().Distance(current_target->transform->GetGlobalPosition()));

	if (distance_snapped < current_attack->info.max_distance_traveled && distance > current_attack->info.min_distance_to_target)
	{
		speed = (current_attack->info.max_distance_traveled - distance_snapped) / snap_time;	
	}
	/*if (player_controller->player_data.type == PlayerController::PlayerType::GERALT)
	{
		if (distance < current_attack->info.max_distance_traveled)
			if (distance < 1.0)
				speed = 0;
			else 
				speed = distance / snap_time;
		else
			speed = (current_attack->info.max_distance_traveled - distance_snapped) / snap_time;
	}
	else
	{
		if (distance > current_attack->info.max_distance_traveled)
			speed = (current_attack->info.max_distance_traveled - distance_snapped) / snap_time;
	}*/
	
	float3 velocity = transform->forward * speed;
	distance_snapped += velocity.Length() * Time::GetDT();

	player_controller->player_data.velocity = velocity;
}

bool PlayerAttacks::FindSnapTarget()
{
	std::vector<ComponentCollider*> colliders_in_range = Physics::OverlapSphere(game_object->transform->GetGlobalPosition(), current_attack->info.snap_detection_range);
	std::vector<GameObject*> enemies_in_range;

	for (auto i = colliders_in_range.begin(); i != colliders_in_range.end(); ++i)
	{
		if (std::strcmp((*i)->game_object_attached->GetTag(), "Enemy") == 0 && !(*i)->game_object_attached->GetComponent<Enemy>()->IsDead())
		{
			enemies_in_range.push_back((*i)->game_object_attached);
		}
	}

	float3 vector = GetMovementVector();
	std::pair<GameObject*, float> snap_candidate = std::pair(nullptr, 1000.0f);

	for(auto it = enemies_in_range.begin(); it != enemies_in_range.end(); ++it)
	{
		float distance = abs(transform->GetGlobalPosition().Distance((*it)->transform->GetGlobalPosition()));
		float angle = math::RadToDeg(vector.AngleBetweenNorm(((*it)->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized()));

		if (angle <= abs(max_snap_angle))
		{
			float snap_value = (angle * snap_angle_value) + (distance * snap_distance_value);
			if (snap_candidate.second > snap_value)
			{
				snap_candidate.first = (*it);
				snap_candidate.second = snap_value;
			}
		}		
	}

	if (snap_candidate.first)
	{
		current_target = snap_candidate.first;
		return true;
	}

	return false;
}

void PlayerAttacks::ReceiveInput(AttackType attack, int spell_index)
{
	next_attack = attack;
	next_spell = spell_index;
}

void PlayerAttacks::CleanUp()
{
	for (auto item = attacks.begin(); item != attacks.end(); ++item) {
		delete (*item);
	}
	attacks.clear();
}

void PlayerAttacks::AttackMovement()
{
	if (FindSnapTarget())
	{
		float frame_time = (float)current_attack->info.activation_frame / (float)player_controller->animator->GetCurrentAnimTPS();
		snap_time = frame_time / player_controller->animator->GetCurrentStateSpeed();
	}
	else
	{
		float3 direction = GetMovementVector();
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		transform->SetGlobalRotation(rot);
		
		player_controller->player_data.velocity = direction * current_attack->info.movement_strength;
	}	
}

void PlayerAttacks::ActivateCollider()
{
	if (current_attack && colliders[(int)current_attack->info.colliders[current_attack->current_collider].type])
	{
		switch (current_attack->info.colliders[current_attack->current_collider].type)
		{
		case Collider_Type::C_Sphere:
		{
			ComponentSphereCollider* sphere_collider = (ComponentSphereCollider*)colliders[(int)current_attack->info.colliders[current_attack->current_collider].type];
			sphere_collider->SetCenter(current_attack->info.colliders[current_attack->current_collider].position);
			sphere_collider->SetRadius(current_attack->info.colliders[current_attack->current_collider].radius);
			sphere_collider->SetRotation(current_attack->info.colliders[current_attack->current_collider].rotation);
		}
			break;
		default:
		{
			ComponentBoxCollider* box_collider = (ComponentBoxCollider*)colliders[(int)current_attack->info.colliders[current_attack->current_collider].type];
			box_collider->SetCenter(current_attack->info.colliders[current_attack->current_collider].position);
			box_collider->SetSize(current_attack->info.colliders[current_attack->current_collider].size);
			box_collider->SetRotation(current_attack->info.colliders[current_attack->current_collider].rotation);
		}
			break;
		}
		colliders[(int)current_attack->info.colliders[current_attack->current_collider].type]->SetEnable(true);
	}
}

void PlayerAttacks::UpdateCollider()
{
	if (current_attack->current_collider + 1 < current_attack->info.colliders.size())
	{
		colliders[(int)current_attack->info.colliders[current_attack->current_collider].type]->SetEnable(false);
		current_attack->current_collider++;
		ActivateCollider();
	}
}

void PlayerAttacks::DeactivateCollider()
{
	for (std::vector<ComponentCollider*>::iterator it = colliders.begin(); it != colliders.end(); ++it)
	{
		(*it)->SetEnable(false);
	}
}

void PlayerAttacks::CastSpell()
{
	if (current_attack)
	{
		LOG("Casting Spell %s", current_attack->info.name.c_str());
		player_controller->PlayAttackParticle();
		player_controller->audio->StartSound(current_attack->info.audio_name.c_str());
		player_controller->player_data.stats["Chaos"].DecreaseStat(current_attack->info.stats["Cost"].GetValue());

		if(player_controller->HUD)
			player_controller->HUD->GetComponent<UI_Char_Frame>()->ManaChange(player_controller->player_data.stats["Chaos"].GetValue(), player_controller->player_data.stats["Chaos"].GetMaxValue());

		if (current_attack->HasTag(Attack_Tags::T_AOE))
		{
			if (GameManager::instance->rumbler_manager)
			{ 
				if (strcmp("Igni", current_attack->info.name.c_str()) == 0)
					GameManager::instance->rumbler_manager->StartRumbler(RumblerType::INCREASING, player_controller->controller_index, 0.2);
				else 
					GameManager::instance->rumbler_manager->StartRumbler(RumblerType::DECREASING, player_controller->controller_index, 0.2);

			}
			
			ActivateCollider();
		}

		if (current_attack->HasTag(Attack_Tags::T_Buff))
		{
			if (GameManager::instance->rumbler_manager)
				GameManager::instance->rumbler_manager->StartRumbler(RumblerType::INCREASING, player_controller->controller_index, 0.2);

			player_controller->AddEffect(GameManager::instance->effects_factory->CreateEffect(current_attack->info.effect));

		}
		if (current_attack->HasTag(Attack_Tags::T_Trap))
		{
			GameObject::Instantiate(current_attack->info.prefab_to_spawn.c_str(), this->transform->GetGlobalPosition());
			if (GameManager::instance->rumbler_manager)
				GameManager::instance->rumbler_manager->StartRumbler(RumblerType::INCREASING, player_controller->controller_index, 0.2);
		}
		if (current_attack->HasTag(Attack_Tags::T_Projectile))
		{
			GameObject* projectile_go = GameObject::Instantiate(current_attack->info.prefab_to_spawn.c_str(),
				player_controller->particle_spawn_positions[1]->transform->GetGlobalPosition());

			float3 direction = current_target ? (current_target->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized() : this->transform->forward;
			Quat rot = projectile_go->transform->GetGlobalRotation();
			rot = rot.LookAt(projectile_go->transform->forward, direction, projectile_go->transform->up, float3::unitY());
			projectile_go->transform->SetGlobalRotation(rot);
			PlayerProjectile* projectile = projectile_go->GetComponent<PlayerProjectile>();
			projectile->direction = direction;
			projectile->player = player_controller;

			if (GameManager::instance->rumbler_manager)
				GameManager::instance->rumbler_manager->StartRumbler(RumblerType::INCREASING, player_controller->controller_index);
		}
	}
}

void PlayerAttacks::OnHit(Enemy* enemy)
{
	current_attack->enemies_hit.push_back(enemy);
	AttackShake();
	if (GameManager::instance->player_manager && !current_attack->HasTag(Attack_Tags::T_Spell) && current_attack->IsLast())
		GameManager::instance->player_manager->IncreaseUltimateCharge(5);

	if (current_attack->HasTag(Attack_Tags::T_Debuff))
	{
		enemy->AddEffect(GameManager::instance->effects_factory->CreateEffect(current_attack->info.effect));
	}
	if (current_attack->HasTag(Attack_Tags::T_Chaining))
	{
		enemy->Stun(1.0f);
		if (current_attack->enemies_hit.size() == 1)
		{
			SpawnChainParticle(this->game_object->transform->GetGlobalPosition() , enemy->transform->GetGlobalPosition());
		}
		std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(enemy->transform->GetGlobalPosition(), current_attack->info.chain_range);

		for (auto it = colliders.begin(); it != colliders.end(); ++it)
		{
			if (strcmp((*it)->game_object_attached->GetTag(), "Enemy") == 0) {
				Enemy* enemy_close = (*it)->game_object_attached->GetComponent<Enemy>();
				if (enemy_close && current_attack->CanHit(enemy_close)) {
					SpawnChainParticle(enemy->transform->GetGlobalPosition(), enemy_close->transform->GetGlobalPosition());
					if (!enemy_close->IsDead())
						enemy_close->GetDamaged(GetCurrentDMG(), player_controller);

					OnHit(enemy_close);
				}
			}
		}
	}

	if (GameManager::instance->rumbler_manager && current_attack->info.name[current_attack->info.name.size() - 1] == 'H')
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::HEAVY_ATTACK, player_controller->controller_index);
	else if (GameManager::instance->rumbler_manager && current_attack->info.name[current_attack->info.name.size() - 1] == 'L')
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::LIGHT_ATTACK, player_controller->controller_index);
}

void PlayerAttacks::AllowCombo()
{
	can_execute_input = true;
	player_controller->SpawnParticle(current_attack->info.allow_combo_p_name, player_controller->particle_spawn_positions[1]->transform->GetLocalPosition());
}

bool PlayerAttacks::CanBeInterrupted()
{
	if (colliders[(int)current_attack->info.colliders[current_attack->current_collider].type])
		return !colliders[(int)current_attack->info.colliders[current_attack->current_collider].type]->IsEnabled();
	else
		return true;
}

float3 PlayerAttacks::GetMovementVector()
{
	float3 direction_vector = float3(player_controller->movement_input.x, 0.f, player_controller->movement_input.y);
	if (!player_controller->mov_input)
		direction_vector = player_controller->transform->forward;
	else
	{
		direction_vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(direction_vector);
		direction_vector.y = 0.f;
	}

	return direction_vector;
}

float3 PlayerAttacks::GetKnockBack(ComponentTransform* enemy_transform)
{
	float3 knockback = float3::zero();

	if (current_attack)
	{
		float3 enemy_direction = (enemy_transform->GetGlobalPosition() - player_controller->game_object->transform->GetGlobalPosition()).Normalized();
		enemy_direction.y = 0;
		if (current_attack->info.knock_direction.z != 0)
		{
			knockback = enemy_direction * current_attack->info.knock_direction.z;
		}
		if (current_attack->info.knock_direction.y != 0)
		{
			knockback += float3::unitY() * current_attack->info.knock_direction.y;
		}
		//float2 tmp_f = float2(player_controller->game_object->transform->forward.x, player_controller->game_object->transform->forward.z);
		//float2 tmp_e = float2(enemy_direction.x, enemy_direction.z);
		//float angle = acos(math::Dot(tmp_f, tmp_e) / (tmp_f.Length() * tmp_e.Length()));

		//knockback = player_controller->game_object->transform->GetGlobalMatrix().MulDir(current_attack->info.knock_direction).Normalized();
		//knockback = Quat::RotateAxisAngle(float3::unitY(), angle) * knockback;

		knockback *= current_attack->info.stats["KnockBack"].GetValue();
	}

	return knockback;
}

void PlayerAttacks::AttackShake()
{
	if (current_attack->info.shake == 1)
	{
		shake->Shake(0.13f, 0.9, 5.f, 0.1f, 0.1f, 0.1f);
		if (GameManager::instance->rumbler_manager)
			GameManager::instance->rumbler_manager->StartRumbler(RumblerType::LAST_ATTACK, player_controller->controller_index);
	}
}

float PlayerAttacks::GetCurrentDMG()
{
	if (!current_attack->HasTag(Attack_Tags::T_Spell))
		return current_attack->info.stats["Damage"].GetValue() * player_controller->player_data.stats["Strength"].GetValue();
	else
		return current_attack->info.stats["Damage"].GetValue();
}

Attack* PlayerAttacks::GetCurrentAttack()
{
	return current_attack;
}

void PlayerAttacks::CreateAttacks()
{
	std::string character_string = player_controller->player_data.type == PlayerController::PlayerType::GERALT ? "Geralt" : "Yennefer";
	std::string json = "GameData/Players/"+ character_string+"Combos.json";

	JSONfilepack* combo = JSONfilepack::GetJSON(json.c_str());

	JSONArraypack* attack_combo = combo->GetArray("Combos");
	if (attack_combo)
	{
		attack_combo->GetFirstNode();
		for (uint i = 0; i < attack_combo->GetArraySize(); i++)
		{
			Attack::AttackInfo info;

			Stat::FillStats(info.stats, attack_combo->GetArray("stats"));
			info.name = attack_combo->GetString("name");
			info.input = attack_combo->GetString("button");
			info.particle_name = attack_combo->GetString("particle_name");
			info.particle_pos = float3(attack_combo->GetNumber("particle_pos.pos_x"),
				attack_combo->GetNumber("particle_pos.pos_y"),
				attack_combo->GetNumber("particle_pos.pos_z"));

			JSONArraypack* colliders_array = attack_combo->GetArray("colliders");
			if(colliders_array)
			{
				colliders_array->GetFirstNode();
				for (uint j = 0; j < colliders_array->GetArraySize(); j++)
				{
					Attack::AttackCollider collider;
					collider.type = GetColliderType(colliders_array->GetString("type"));
					if (collider.type == Collider_Type::C_Sphere)
					{
						collider.position = float3(colliders_array->GetNumber("pos_x"),
							colliders_array->GetNumber("pos_y"),
							colliders_array->GetNumber("pos_z"));
						collider.radius = colliders_array->GetNumber("radius");
						collider.rotation = float3(colliders_array->GetNumber("rot_x"),
							colliders_array->GetNumber("rot_y"),
							colliders_array->GetNumber("rot_z"));
					}
					else
					{
						collider.position = float3(colliders_array->GetNumber("pos_x"),
							colliders_array->GetNumber("pos_y"),
							colliders_array->GetNumber("pos_z"));
						collider.size = float3(colliders_array->GetNumber("width"),
							colliders_array->GetNumber("height"),
							colliders_array->GetNumber("depth"));
						collider.rotation = float3(colliders_array->GetNumber("rot_x"),
							colliders_array->GetNumber("rot_y"),
							colliders_array->GetNumber("rot_z"));
					}

					info.colliders.push_back(collider);
					colliders_array->GetAnotherNode();
				}
			}

			info.knock_direction = float3(attack_combo->GetNumber("knock_back.x"),
				attack_combo->GetNumber("knock_back.y"),
				attack_combo->GetNumber("knock_back.z"));

			info.freeze_time = attack_combo->GetNumber("freeze_time");
			info.movement_strength = attack_combo->GetNumber("movement_strength");
			info.activation_frame = attack_combo->GetNumber("activation_frame");
			info.max_distance_traveled = attack_combo->GetNumber("max_distance_traveled");
			info.next_light = attack_combo->GetString("next_attack_light");
			info.next_heavy = attack_combo->GetString("next_attack_heavy");
			info.shake = attack_combo->GetNumber("cam_shake");
			info.allow_combo_p_name = attack_combo->GetString("allow_particle");
			info.snap_detection_range = attack_combo->GetNumber("snap_detection_range");
			info.min_distance_to_target = attack_combo->GetNumber("min_distance_to_target");

			Attack* attack = new Attack(info);
			attacks.push_back(attack);

			attack_combo->GetAnotherNode();
		}
		ConnectAttacks();
	}

	JSONArraypack* spells_json = combo->GetArray("Spells");
	if (spells_json)
	{
		spells_json->GetFirstNode();
		for (uint i = 0; i < spells_json->GetArraySize(); i++)
		{
			Attack::AttackInfo info;

			Stat::FillStats(info.stats, spells_json->GetArray("stats"));
			info.name = spells_json->GetString("name");
			info.particle_name = spells_json->GetString("particle_name");
			info.particle_pos = float3(spells_json->GetNumber("particle_pos.pos_x"),
				spells_json->GetNumber("particle_pos.pos_y"),
				spells_json->GetNumber("particle_pos.pos_z"));

			JSONArraypack* colliders_array = spells_json->GetArray("colliders");
			if (colliders_array)
			{
				colliders_array->GetFirstNode();
				for (uint j = 0; j < colliders_array->GetArraySize(); j++)
				{
					Attack::AttackCollider collider;
					collider.type = GetColliderType(colliders_array->GetString("type"));
					if (collider.type == Collider_Type::C_Sphere)
					{
						collider.position = float3(colliders_array->GetNumber("pos_x"),
							colliders_array->GetNumber("pos_y"),
							colliders_array->GetNumber("pos_z"));
						collider.radius = colliders_array->GetNumber("radius");
						collider.rotation = float3(colliders_array->GetNumber("rot_x"),
							colliders_array->GetNumber("rot_y"),
							colliders_array->GetNumber("rot_z"));
					}
					else
					{
						collider.position = float3(colliders_array->GetNumber("pos_x"),
							colliders_array->GetNumber("pos_y"),
							colliders_array->GetNumber("pos_z"));
						collider.size = float3(colliders_array->GetNumber("width"),
							colliders_array->GetNumber("height"),
							colliders_array->GetNumber("depth"));
						collider.rotation = float3(colliders_array->GetNumber("rot_x"),
							colliders_array->GetNumber("rot_y"),
							colliders_array->GetNumber("rot_z"));
					}

					info.colliders.push_back(collider);
					colliders_array->GetAnotherNode();
				}
			}

			info.movement_strength = spells_json->GetNumber("movement_strength");
			info.activation_frame = spells_json->GetNumber("activation_frame");
			info.max_distance_traveled = spells_json->GetNumber("max_distance_traveled");
			info.freeze_time = spells_json->GetNumber("freeze_time");
			info.effect = spells_json->GetString("effect");
			info.prefab_to_spawn = spells_json->GetString("prefab_to_spawn");
			info.snap_detection_range = spells_json->GetNumber("snap_detection_range");
			info.min_distance_to_target = spells_json->GetNumber("min_distance_to_target");
			info.audio_name = spells_json->GetString("audio_name");
			info.knock_direction = float3(spells_json->GetNumber("knock_back.x"),
				spells_json->GetNumber("knock_back.y"),
				spells_json->GetNumber("knock_back.z"));


			JSONArraypack* tags = spells_json->GetArray("tags");
			uint num_tags = tags->GetArraySize();
			if (tags)
			{
				tags->GetFirstNode();
				for (uint j = 0; j < num_tags; j++)
				{
					std::string tag_str = tags->GetString("tag");
					Attack_Tags tag = GetTag(tag_str);
					if (tag == Attack_Tags::T_Chaining)
					{
						info.chain_range = spells_json->GetNumber("chain_range");
						info.chain_particle = spells_json->GetString("chain_particle");
					}	
					info.tags.push_back(tag);

					tags->GetAnotherNode();
				}
			}
			
			Attack* attack = new Attack(info);
			spells.push_back(attack);

			spells_json->GetAnotherNode();
		}
	}
	JSONfilepack::FreeJSON(combo);
}

void PlayerAttacks::ConnectAttacks()
{
	for (auto it_attack = attacks.begin(); it_attack != attacks.end(); it_attack++)
	{
		if ((*it_attack)->info.name == "H")
			base_heavy_attack = (*it_attack);
		else if ((*it_attack)->info.name == "L")
			base_light_attack = (*it_attack);

		std::string n_light = (*it_attack)->info.next_light.c_str();
		std::string n_heavy = (*it_attack)->info.next_heavy.c_str();

		if (n_light == "End" && n_heavy == "End")
			continue;

		for (auto it_next = attacks.begin(); it_next != attacks.end(); it_next++)
		{
			if (n_light == (*it_next)->info.name)
			{
				(*it_attack)->light_attack_link = (*it_next);
			}
			if (n_heavy == (*it_next)->info.name)
			{
				(*it_attack)->heavy_attack_link = (*it_next);
			}
		}
	}
}

void PlayerAttacks::SpawnChainParticle(float3 from, float3 to)
{
	float3 mid_point = float3((from.x + to.x) / 2, (from.y + to.y) / 2, (from.z + to.z) / 2);
	mid_point.y += 0.5f; //hardcode lol
	float distance = from.DistanceSq(to);
	
	float3 direction = (to - from).Normalized();
	GameObject* new_particle = GameManager::instance->particle_pool->GetInstance(current_attack->info.chain_particle, mid_point);
	if (new_particle == nullptr)
		return;

	Quat rot = new_particle->transform->GetGlobalRotation().LookAt(new_particle->transform->forward, direction, new_particle->transform->up, float3::unitY());
	new_particle->transform->SetGlobalRotation(rot);
	//new_particle->GetChild(0)->transform->SetGlobalRotation(rot);
	//float angle = math::RadToDeg(eu_rot.y);

	ComponentParticleSystem* p_system = new_particle->GetComponent<ComponentParticleSystem>();
	p_system->GetSystem()->SetParticleInitialSize(float3(1, distance * 0.3f, 1));
	ComponentParticleSystem* p_system_2 = new_particle->GetComponentInChildren<ComponentParticleSystem>();
	p_system_2->GetSystem()->SetParticleInitialSize(float3(1, distance * 0.3f, 1));
	//p_system->GetSystem()->SetParticleInitialAngle(float3(0, direction.x > 0 ? angle:-angle, 90));

	player_controller->particles.push_back(new_particle);
}
