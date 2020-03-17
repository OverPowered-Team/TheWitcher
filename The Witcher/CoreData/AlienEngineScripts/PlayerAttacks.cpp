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

	Attack* attack = new Attack();
	Attack* tmp_attack = attack;
	attack->name = "XXX";
	attacks.push_back(attack);

	attack = new Attack();
	attack->name = "XX";
	attack->light_attack_link = tmp_attack;
	tmp_attack = attack;
	attacks.push_back(attack);

	attack = new Attack();
	attack->name = "X";
	attack->light_attack_link = tmp_attack;
	base_light_attack = attack;
	attacks.push_back(attack);
}

void PlayerAttacks::ActiveCollider()
{
	LOG("COLLIDER ACTIVED");
}

void PlayerAttacks::DesactiveCollider()
{
	LOG("COLLIDER DESACTIVED");
}
