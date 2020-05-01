#include "Bonfire.h"

Bonfire::Bonfire() : Alien()
{
}

Bonfire::~Bonfire()
{
}

void Bonfire::Start()
{
	ui_bonfire = game_object->GetChild("Interact");
	checkpoint = game_object->GetChild("Checkpoint");
}

void Bonfire::SetBonfireUsed(int player_index)
{
	if ((first_player_index != player_index) && first_player_index != -1)
	{
		game_object->GetChild("Model_xd")->GetChild("Logs")->GetChild("Fire")->SetEnable(false);
		is_active = false;
	}
	else
	{
		first_player_index = player_index;
	}
}

bool Bonfire::HaveThisPlayerUsedThis(int controller_index)
{
	return controller_index == first_player_index;
}
