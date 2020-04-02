#include "DebugManager.h"

DebugManager::DebugManager() : Alien()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Start()
{
}

void DebugManager::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_LALT))
	{
		if (Input::GetKeyDown(SDL_SCANCODE_M))
		{
			SceneManager::LoadScene("Main_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_W))
		{
			SceneManager::LoadScene("Win");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_L))
		{
			SceneManager::LoadScene("Lose");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_D))
		{
			
		}
	}

}