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
	CreateAttacks();
	player_controller = (PlayerController*)GetComponentScript("PlayerController");
}

void PlayerAttacks::StartAttack()
{
	LOG("START ATTACK");
	SelectAttack();
	DoAttack();
}

void PlayerAttacks::UpdateAttack()
{
	LOG("UPDATE ATTACK");
	if (last_input != ' ')
	{
		StartAttack();
	}
	else if (!player_controller->animator->IsPlaying(current_attack->name.c_str()))
	{
		LOG("NO NEXT ATTACK");
		current_attack = nullptr;
		player_controller->state = PlayerController::PlayerState::IDLE;			
	}
}

void PlayerAttacks::SaveInput(char input)
{
	/*if (current_attack)
	{
		if(CanReceiveInput())
			last_input = input;
	}
	else*/
		last_input = input;
}

void PlayerAttacks::DoAttack()
{
	LOG("DO ATTACK %s", current_attack->name.c_str());
	player_controller->animator->PlayState(current_attack->name.c_str());

	/*float start_time = Time::GetGameTime();
	float final_time = start_time + player_controller->animator->GetCurrentStateDuration();
	attack_input_time = (start_time - final_time) - input_window;*/
}
void PlayerAttacks::SelectAttack()
{
	LOG("SELECT ATTACK");
	LOG("INPUT IS %s", std::to_string(last_input).c_str());
	if (!current_attack)
	{
		if(last_input == 'X')
			current_attack = base_light_attack;
		else
			current_attack = base_heavy_attack;
	}
	else
	{
		if (last_input == 'X')
		{
			if (current_attack->light_attack_link)
				current_attack = current_attack->light_attack_link;
			else
				current_attack = base_light_attack;
		}
		else if(last_input == 'Y')
		{
			if (current_attack->heavy_attack_link)
				current_attack = current_attack->heavy_attack_link;
			else
				current_attack = base_heavy_attack;
		}		
	}

	last_input = ' ';
}
bool PlayerAttacks::CanReceiveInput()
{
	return (Time::GetGameTime() > attack_input_time);
}
void PlayerAttacks::CreateAttacks()
{
	LOG("CREATE ATTACKS");

	JSON_Value* value = json_parse_file("Configuration/GeraltCombos.json");
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* combo = new JSONfilepack("Configuration/GeraltCombos.json", object, value);
		
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
				std::string next_attack = attack_combo->GetString("next_attack");
				Attack* attack = new Attack(attack_name.data(), button_name.data(), pos, size, multiplier);
				attacks.push_back(attack);
			}
		}
		
		delete[] attack_combo;
		delete combo;
		delete object;
		delete value;
	}
}

void PlayerAttacks::ActiveCollider()
{
	LOG("COLLIDER ACTIVED");
}

void PlayerAttacks::DesactiveCollider()
{
	LOG("COLLIDER DESACTIVED");
}
