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
