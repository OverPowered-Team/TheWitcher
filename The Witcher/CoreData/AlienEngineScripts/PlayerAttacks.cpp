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

void PlayerAttacks::StartAttack(AttackType attack)
{
	LOG("START ATTACK");
	SelectAttack(attack);
	DoAttack();
}

void PlayerAttacks::UpdateAttack(AttackType new_attack)
{
	LOG("UPDATE ATTACK");
	if (new_attack != AttackType::NONE && CanReceiveInput())
	{
		StartAttack(new_attack);
	}
	else if (Time::GetGameTime() > final_attack_time)
	{
		LOG("NO NEXT ATTACK");
		current_attack = nullptr;
		player_controller->state = PlayerController::PlayerState::IDLE;			
	}
}

void PlayerAttacks::DoAttack()
{
	LOG("DO ATTACK %s", current_attack->name.c_str());
	player_controller->animator->PlayState(current_attack->name.c_str());

	float start_time = Time::GetGameTime();
	//final_attack_time = start_time + player_controller->animator->GetCurrentStateDuration();
	attack_input_time = (start_time - final_attack_time) - input_window;
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
