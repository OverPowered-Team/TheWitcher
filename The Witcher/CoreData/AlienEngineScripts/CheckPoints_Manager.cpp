#include "CheckPoints_Manager.h"
#include "CheckPoint.h"
#include "PlayerManager.h"
#include "PlayerController.h"

std::vector<CheckPoint*> CheckPoints_Manager::checkpoints;

CheckPoints_Manager::CheckPoints_Manager() : Alien()
{
}

CheckPoints_Manager::~CheckPoints_Manager()
{
}

void CheckPoints_Manager::Start()
{
	last_checkpoint = checkpoints.front();
}

void CheckPoints_Manager::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_F11))
	{
		TP(1);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_F10))
	{
		TP(-1);
	}
}

void CheckPoints_Manager::TP(int i)
{
	for (uint i = 0; i < 2; ++i)
	{
		((PlayerManager*)GameObject::FindWithName("GameManager")->
			GetComponentScript("PlayerManager"))->players[i]->transform->SetGlobalPosition(last_checkpoint->transform->GetGlobalPosition());
	}

	std::vector<CheckPoint*>::iterator iter = checkpoints.begin();
	for (; iter != checkpoints.end(); ++iter)
	{
		if ((*iter) == last_checkpoint)
		{
			last_checkpoint = *(iter + i);
			break;
		}
	}
}
