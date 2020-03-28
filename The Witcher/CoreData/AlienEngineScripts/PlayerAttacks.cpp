#include "PlayerController.h"
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

	CreateAttacks();
}

void PlayerAttacks::StartAttack(AttackType attack)
{
	LOG("START ATTACK");
	SelectAttack(attack);
	DoAttack();
}

void PlayerAttacks::ComboAttack()
{
	LOG("UPDATE ATTACK");
	if (Time::GetGameTime() > finish_attack_time)
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
	LOG("DO ATTACK %s", current_attack->name.c_str());
	player_controller->animator->PlayState(current_attack->name.c_str());
	can_execute_input = false;
	next_attack = AttackType::NONE;
	player_controller->controller->ApplyImpulse(GetAttackImpulse());

	float start_time = Time::GetGameTime();
	float animation_duration = player_controller->animator->GetCurrentStateDuration();
	LOG("ANIM DURATION IS %f", animation_duration);
	finish_attack_time = start_time + animation_duration;
}

void PlayerAttacks::SelectAttack(AttackType attack)
{
	LOG("SELECT ATTACK");
	LOG("INPUT IS %s", attack == AttackType::LIGHT? "LIGHT ATTACK":"HEAVY ATTACK");
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
void PlayerAttacks::ReceiveInput(AttackType attack)
{
	next_attack = attack;
}
/*bool PlayerAttacks::CanReceiveInput()
{
	return (Time::GetGameTime() > attack_input_time && (current_attack->heavy_attack_link != nullptr || current_attack->light_attack_link != nullptr));
}*/

void PlayerAttacks::CreateAttacks()
{
	LOG("CREATE ATTACKS");

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
			std::string n_light = attack_combo->GetString("next_attack_light");
			std::string n_heavy = attack_combo->GetString("next_attack_heavy");

			Attack* attack = new Attack(attack_name.data(), button_name.data(), pos, size, multiplier, movement_strength, n_light.data(), n_heavy.data());
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

void PlayerAttacks::ActivateCollider()
{
	LOG("COLLIDER ACTIVED");
	if (collider)
	{
		collider->SetCenter(current_attack->collider_position);
		collider->SetSize(current_attack->collider_size);
		collider->SetEnable(true);
	}
}

void PlayerAttacks::DeactivateCollider()
{
	LOG("COLLIDER DESACTIVED");
	if(collider)
		collider->SetEnable(false);

	can_execute_input = true;
}

void PlayerAttacks::AllowCombo()
{
	can_execute_input = true;
}

bool PlayerAttacks::CanBeInterrupted()
{
	LOG("CHECKING IF CAN BE INTERRUPTED");
	if (collider)
		return !collider->IsEnabled();
	else
		return true;
}

float3 PlayerAttacks::GetAttackImpulse()
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

	vector *= current_attack->movement_strength;
	//raycast here


	LOG("ATTACK IMPULSE IS (%f, %f, %f)", vector.x, vector.y, vector.z);

	return vector;
}

void PlayerAttacks::OnAnimationEnd(const char* name) {
	if (current_attack)
	{
		LOG("NO NEXT ATTACK");
		current_attack = nullptr;
	}
}