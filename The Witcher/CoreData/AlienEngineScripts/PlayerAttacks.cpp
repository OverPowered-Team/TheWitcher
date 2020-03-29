#include "PlayerController.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "PlayerAttacks.h"

PlayerAttacks::PlayerAttacks() : Alien()
{
}

PlayerAttacks::~PlayerAttacks()
{
}

void PlayerAttacks::Start()
{
	player_controller = (PlayerController*)GetComponentScript("PlayerController");
	collider = (ComponentBoxCollider*)collider_go->GetComponent(ComponentType::BOX_COLLIDER);

	//temporary
	enemies_size = GameObject::FindGameObjectsWithTag("Enemy", &enemies);

	CreateAttacks();
}

void PlayerAttacks::StartAttack(AttackType attack)
{
	SelectAttack(attack);
	DoAttack();
}

void PlayerAttacks::UpdateCurrentAttack()
{
	if (current_target && transform->GetGlobalPosition().Distance(current_target->transform->GetGlobalPosition()) > min_snap_range)
		player_controller->controller->SetWalkDirection(CalculateSnapVelocity());
	else
		player_controller->controller->SetWalkDirection(float3::zero());

	if (Time::GetGameTime() >= finish_attack_time)
	{
		if (abs(player_controller->player_data.currentSpeed) < 0.01F)
			player_controller->state = PlayerController::PlayerState::IDLE;
		if (abs(player_controller->player_data.currentSpeed) > 0.01F)
			player_controller->state = PlayerController::PlayerState::RUNNING;
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

	player_controller->animator->PlayState(current_attack->name.c_str());
	AttackMovement();

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
}

std::vector<std::string> PlayerAttacks::GetFinalAttacks()
{
	std::vector<std::string> final_attacks;

	for (std::vector<Attack*>::iterator it = attacks.begin(); it != attacks.end(); ++it)
	{
		if ((*it)->heavy_attack_link == nullptr && (*it)->light_attack_link == nullptr)
			final_attacks.push_back((*it)->name);
	}
	return final_attacks;
}

void PlayerAttacks::OnAddAttackEffect(std::string _attack_name)
{
	for (std::vector<Attack*>::iterator it = attacks.begin(); it != attacks.end(); ++it)
	{
		if ((*it)->name == _attack_name)
		{
			(*it)->base_damage->CalculateStat(player_controller->effects);
			//(*it)->base_range.CalculateStat(player_controller->effects);
		}
	}
}


bool PlayerAttacks::FindSnapTarget()
{
	std::vector<GameObject*> possible_targets;
	for(uint i = 0; i < enemies_size; ++i)
	{
		float distance = enemies[i]->transform->GetGlobalPosition().Distance(transform->GetGlobalPosition());
		if (distance <= snap_range)
			possible_targets.push_back(enemies[i]);
	}

	float3 vector = GetMovementVector();
	for (std::vector<GameObject*>::iterator it = possible_targets.begin(); it != possible_targets.end(); ++it)
	{
		float angle = vector.AngleBetween((*it)->transform->GetGlobalPosition() - transform->GetGlobalPosition());
		if (angle <= math::DegToRad(max_snap_angle))
		{
			current_target = (*it);
			return true;
		}		
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
		(*item)->CleanUp();
		delete (*item);
	}
	attacks.clear();
	//GameObject::FreeArrayMemory((void***)&enemies);
}

float3 PlayerAttacks::CalculateSnapVelocity()
{
	float speed = transform->GetGlobalPosition().Distance(current_target->transform->GetGlobalPosition()) / snap_time;
	float3 velocity = transform->forward * speed * Time::GetDT();

	return velocity;
}

void PlayerAttacks::AttackMovement()
{
	if (FindSnapTarget())
	{
		float frame_time = (float)current_attack->activation_frame / (float)player_controller->animator->GetCurrentAnimTPS();
		snap_time = frame_time / player_controller->animator->GetCurrentStateSpeed();

		float3 direction = (current_target->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		player_controller->controller->SetRotation(rot);
	}
	else
	{
		float3 direction = GetMovementVector();
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());

		float3 impulse = direction * current_attack->movement_strength;
		player_controller->controller->ApplyImpulse(impulse);
		player_controller->controller->SetRotation(rot);
	}	
}

void PlayerAttacks::ActivateCollider()
{
	if (collider && current_attack)
	{
		collider->SetCenter(current_attack->collider_position);
		collider->SetSize(current_attack->collider_size);
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

void PlayerAttacks::OnDrawGizmos()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), snap_range, Color::Cyan()); //snap_range
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
	float3 vector = float3(Input::GetControllerHoritzontalLeftAxis(player_controller->controller_index), 0.f,
		Input::GetControllerVerticalLeftAxis(player_controller->controller_index));

	if (vector.Length() < player_controller->stick_threshold)
		vector = player_controller->transform->forward;
	else
	{
		vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(vector);
		vector.y = 0.f;
		vector.Normalize();
	}

	return vector;
}

void PlayerAttacks::OnAnimationEnd(const char* name) {
	if (current_attack)
	{
		current_attack = nullptr;
	}
}

void PlayerAttacks::CreateAttacks()
{
	JSONfilepack* combo = JSONfilepack::GetJSON("Configuration/GeraltCombos.json");

	JSONArraypack* attack_combo = combo->GetArray("Combos");
	if (attack_combo)
	{
		attack_combo->GetFirstNode();
		for (uint i = 0; i < attack_combo->GetArraySize(); i++)
		{
			std::string attack_name = attack_combo->GetString("name");
			std::string button_name = attack_combo->GetString("button");

			float3 pos = float3(attack_combo->GetNumber("collider.pos_x"),
				attack_combo->GetNumber("collider.pos_y"),
				attack_combo->GetNumber("collider.pos_z"));
			float3 size = float3(attack_combo->GetNumber("collider.width"),
				attack_combo->GetNumber("collider.height"),
				attack_combo->GetNumber("collider.depth"));
			float multiplier = attack_combo->GetNumber("multiplier");
			float movement_strength = attack_combo->GetNumber("movement_strength");
			int activation_frame = attack_combo->GetNumber("activation_frame");
			std::string n_light = attack_combo->GetString("next_attack_light");
			std::string n_heavy = attack_combo->GetString("next_attack_heavy");

			Attack* attack = new Attack(attack_name.data(), button_name.data(), pos, size, multiplier, activation_frame, movement_strength, n_light.data(), n_heavy.data());
			attacks.push_back(attack);

			attack_combo->GetAnotherNode();
		}
		ConnectAttacks();
	}
	JSONfilepack::FreeJSON(combo);
}

void PlayerAttacks::ConnectAttacks()
{
	for (auto it_attack = attacks.begin(); it_attack != attacks.end(); it_attack++)
	{
		if ((*it_attack)->name == "H")
			base_heavy_attack = (*it_attack);
		else if ((*it_attack)->name == "L")
			base_light_attack = (*it_attack);

		std::string n_light = (*it_attack)->next_light.c_str();
		std::string n_heavy = (*it_attack)->next_heavy.c_str();

		if (n_light == "End" && n_heavy == "End")
			continue;

		for (auto it_next = attacks.begin(); it_next != attacks.end(); it_next++)
		{
			if (n_light == (*it_next)->name)
			{
				(*it_attack)->light_attack_link = (*it_next);
			}
			if (n_heavy == (*it_next)->name)
			{
				(*it_attack)->heavy_attack_link = (*it_next);
			}
		}
	}
}

void Attack::CleanUp()
{
	delete base_damage;
}
