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

void Bonfire::SetBonfireUsed(PlayerController* player)
{
	if ((first_player != player) && player != nullptr)
	{
		game_object->GetChild("Model_xd")->GetChild("Logs")->GetChild("Fire")->SetEnable(false);
		is_active = false;
	}
	else
	{
		first_player = player;
	}
}

bool Bonfire::HaveThisPlayerUsedThis(PlayerController* player)
{
	return first_player == player;
}
