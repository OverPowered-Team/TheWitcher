#include "PlayerController.h"
#include "State.h"
#include "Effect.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerAttacks.h"
#include "CameraShake.h"
PlayerAttacks::PlayerAttacks() : Alien()
{
}

PlayerAttacks::~PlayerAttacks()
{
}

void PlayerAttacks::Start()
{
	player_controller = GetComponent<PlayerController>();
	collider = game_object->GetChild("Attacks_Collider")->GetComponent<ComponentBoxCollider>();
	collider->SetEnable(false);
	
	shake = Camera::GetCurrentCamera()->game_object_attached->GetComponent<CameraShake>();

	CreateAttacks();
}

void PlayerAttacks::StartAttack(AttackType attack)
{
	SelectAttack(attack);
	DoAttack();
	AttackMovement();
}

void PlayerAttacks::StartSpell(uint spell_index)
{
	if (spell_index < spells.size())
	{
		current_attack = spells[spell_index];
		DoAttack();
		AttackMovement();
	}
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
		player_controller->player_data.speed += player_controller->player_data.speed * player_controller->player_data.slow_speed * Time::GetDT(); //SLOW DOWN
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
		StartAttack(next_attack);
	}
}

void PlayerAttacks::DoAttack()
{
	//Reset attack variables
	can_execute_input = false;
	next_attack = AttackType::NONE;
	current_target = nullptr;
	distance_snapped = 0.0f;

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

	/*if(current_attack && current_attack->IsLast())
		GameManager::instance->player_manager->IncreaseUltimateCharge(5);*/
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
			(*it)->info.base_damage.ApplyEffect(new_effect);
		}
	}
}

void PlayerAttacks::CancelAttack()
{
	current_attack = nullptr;
	collider->SetEnable(false);
	player_controller->player_data.speed = float3::zero();
}

void PlayerAttacks::SnapToTarget()
{
	float3 direction = (current_target->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	float speed = 0.0f;
	float distance = transform->GetGlobalPosition().Distance(current_target->transform->GetGlobalPosition());

	if (player_controller->player_data.type == PlayerController::PlayerType::GERALT)
	{
		if (distance < current_attack->info.max_snap_distance)
			if (distance < 1.0)
				speed = 0;
			else 
				speed = distance / snap_time;
		else
			speed = (current_attack->info.max_snap_distance - distance_snapped) / snap_time;
	}
	else
	{
		if (distance > current_attack->info.max_snap_distance)
			speed = (current_attack->info.max_snap_distance - distance_snapped) / snap_time;
	}

	float3 velocity = transform->forward * speed * Time::GetDT();
	distance_snapped += velocity.Length();

	player_controller->transform->SetGlobalRotation(rot);
	player_controller->player_data.speed = velocity;
}

bool PlayerAttacks::FindSnapTarget()
{
	std::vector<ComponentCollider*> colliders_in_range = Physics::OverlapSphere(game_object->transform->GetGlobalPosition(), snap_detection_range);
	std::vector<GameObject*> enemies_in_range;

	for (auto i = colliders_in_range.begin(); i != colliders_in_range.end(); ++i)
	{
		if (std::strcmp((*i)->game_object_attached->GetTag(), "Enemy") == 0)
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

		if (distance <= snap_detection_range && angle <= abs(max_snap_angle))
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

void PlayerAttacks::ReceiveInput(AttackType attack)
{
	next_attack = attack;
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
		
		player_controller->player_data.speed = direction * current_attack->info.movement_strength;
	}	
}

void PlayerAttacks::ActivateCollider()
{
	if (collider && current_attack)
	{
		collider->SetCenter(current_attack->info.collider_position);
		collider->SetSize(current_attack->info.collider_size);
		collider->SetEnable(true);
	}
}

void PlayerAttacks::DeactivateCollider()
{
	if(collider)
		collider->SetEnable(false);
}

void PlayerAttacks::AllowCombo()
{
	can_execute_input = true;
}

void PlayerAttacks::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), snap_detection_range, Color::Red()); //snap_range
}

bool PlayerAttacks::CanBeInterrupted()
{
	if (collider)
		return !collider->IsEnabled();
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

void PlayerAttacks::AttackShake()
{
	if (current_attack->info.shake == 1)
		shake->Shake(0.13f, 0.9, 5.f,  0.1f, 0.1f, 0.1f);
}

float PlayerAttacks::GetCurrentDMG()
{
	if (player_controller->state->type == StateType::ATTACKING)
		return current_attack->info.base_damage.GetValue() * player_controller->player_data.stats["Strength"].GetValue();
	else
		return current_attack->info.base_damage.GetValue();
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

			info.name = attack_combo->GetString("name");
			info.input = attack_combo->GetString("button");
			info.particle_name = attack_combo->GetString("particle_name");
			info.collider_position = float3(attack_combo->GetNumber("collider.pos_x"),
				attack_combo->GetNumber("collider.pos_y"),
				attack_combo->GetNumber("collider.pos_z"));
			info.collider_size = float3(attack_combo->GetNumber("collider.width"),
				attack_combo->GetNumber("collider.height"),
				attack_combo->GetNumber("collider.depth"));
			info.base_damage = Stat("Attack_Damage", attack_combo->GetNumber("base_damage"));
			info.freeze_time = attack_combo->GetNumber("freeze_time");
			info.movement_strength = attack_combo->GetNumber("movement_strength");
			info.activation_frame = attack_combo->GetNumber("activation_frame");
			info.max_snap_distance = attack_combo->GetNumber("max_snap_distance");
			info.next_light = attack_combo->GetString("next_attack_light");
			info.next_heavy = attack_combo->GetString("next_attack_heavy");
			info.shake = attack_combo->GetNumber("cam_shake");
			Attack* attack = new Attack(info);
			attacks.push_back(attack);

			attack_combo->GetAnotherNode();
		}
		ConnectAttacks();
	}
	JSONArraypack* spells_json = combo->GetArray("Spells");
	if (spells_json)
	{
		Attack::AttackInfo info;

		info.name = spells_json->GetString("name");
		info.particle_name = spells_json->GetString("particle_name");
		info.collider_position = float3(spells_json->GetNumber("collider.pos_x"),
			spells_json->GetNumber("collider.pos_y"),
			spells_json->GetNumber("collider.pos_z"));
		info.collider_size = float3(spells_json->GetNumber("collider.width"),
			spells_json->GetNumber("collider.height"),
			spells_json->GetNumber("collider.depth"));
		info.base_damage = Stat("Attack_Damage", spells_json->GetNumber("base_damage"));
		info.movement_strength = spells_json->GetNumber("movement_strength");
		info.activation_frame = spells_json->GetNumber("activation_frame");
		info.max_snap_distance = spells_json->GetNumber("max_snap_distance");
		info.freeze_time = spells_json->GetNumber("freeze_time");

		Attack* attack = new Attack(info);
		spells.push_back(attack);

		spells_json->GetAnotherNode();
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

