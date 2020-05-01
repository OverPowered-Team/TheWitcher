#include "Bonfire.h"
#include "GameManager.h"
#include "PlayerManager.h"

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
