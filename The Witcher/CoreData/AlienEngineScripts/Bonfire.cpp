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
}

void Bonfire::OnTriggerEnter(ComponentCollider* collider)
{

}

void Bonfire::ManageTrigger()
{

}