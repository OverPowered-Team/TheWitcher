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
	if (!checkpoints.empty())
	{
		last_checkpoint = checkpoints.front();
	}
}

void CheckPoints_Manager::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_F1))
	{
		TP(true);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_F2))
	{
		TP(false);
	}
}

void CheckPoints_Manager::TP(bool is_next)
{
	for (uint i = 0; i < 2; ++i)
	{
		GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players[i]->controller->SetPosition(last_checkpoint->transform->GetLocalPosition());
	}

	std::vector<CheckPoint*>::iterator iter = checkpoints.begin();
	for (; iter != checkpoints.end(); ++iter)
	{
		if ((*iter) == last_checkpoint)
		{
			if (is_next)
			{
				if ((*iter) == checkpoints.back())
				{
					last_checkpoint = checkpoints.front();
				}
				else
				{
					last_checkpoint = *(iter + 1);
				}
			}

			else 
			{
				if ((*iter) == checkpoints.front())
				{
					last_checkpoint = checkpoints.back();
				}
				else
				{
					last_checkpoint = *(iter - 1);
				}
			}
			break;
		}
	}
}
