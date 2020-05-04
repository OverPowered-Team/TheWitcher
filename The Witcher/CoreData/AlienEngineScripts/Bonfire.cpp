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
	game_object->GetChild("Model_xd")->GetChild("Logs")->GetChild("Fire")->SetEnable(false);
}

void Bonfire::SetBonfireUsed()
{
	game_object->GetChild("Model_xd")->GetChild("Logs")->GetChild("Fire")->SetEnable(true);
	has_been_used = true;
}

