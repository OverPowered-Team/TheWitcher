#include "DebugManager.h"
#include "PlayerController.h"

DebugManager::DebugManager() : Alien()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Start()
{
	geralt_controller = (PlayerController*)GameObject::FindWithName("Geralt")->GetComponentScript("PlayerController");
	yennefer_controller = (PlayerController*)GameObject::FindWithName("Yennefer")->GetComponentScript("PlayerController");
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
		if (Input::GetKeyDown(SDL_SCANCODE_G))
		{
			geralt_controller->godmode = !geralt_controller->godmode;
			yennefer_controller->godmode = !yennefer_controller->godmode;
			GameObject::FindWithName("Godmode")->SetEnable(!GameObject::FindWithName("Godmode")->IsEnabled());
		}
	}

}