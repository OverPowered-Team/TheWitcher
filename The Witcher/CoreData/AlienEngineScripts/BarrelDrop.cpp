#include "BarrelDrop.h"
#include "ExplodeChildren.h"
#include "CameraMovement.h"
#include "PlayerController.h"

BarrelDrop::BarrelDrop() : Alien()
{
}

BarrelDrop::~BarrelDrop()
{
}

void BarrelDrop::Start()
{
	barrel = game_object->GetComponent<ExplodeChildren>();
	camera = Camera::GetCurrentCamera()->game_object_attached->GetComponent<CameraMovement>();
	p1 = camera->players[0]->GetComponent<PlayerController>();
	p2 = camera->players[1]->GetComponent<PlayerController>();
}

void BarrelDrop::Update()
{
	if (barrel->has_broke)
		DropPickUp();

	if(Input::GetKeyDown(SDL_SCANCODE_M))
		DropPickUp();
}

void BarrelDrop::DropPickUp()
{
	bool spawn = false;
	int rando = Random::GetRandomIntBetweenTwo(0, 1);
	barrel->has_broke = false;
	if (rando == 0)
	{
		if (p1->player_data.stats["Health"].GetValue() <= 30 || p1->player_data.stats["Health"].GetValue() <= 30) //Low life, spawn life orb
			spawn = RandomDrop(3, 10);
		else 
			spawn = RandomDrop(5, 10);

		if(spawn)
			GameObject::Instantiate("Health_Orb", game_object->transform->GetGlobalPosition());

		return;
	}
	else
	{
		spawn = RandomDrop(4, 10);
		if (spawn)
			GameObject::Instantiate("Coin", game_object->transform->GetGlobalPosition());
		return;
	}
	

	
}

bool BarrelDrop::RandomDrop(const int min, const int max)
{
	int rate = Random::GetRandomIntBetweenTwo(1, 10);
	if (rate > min && rate < max)
		return true;

	return false;
}


