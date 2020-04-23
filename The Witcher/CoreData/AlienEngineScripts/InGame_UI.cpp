#include "InGame_UI.h"

InGame_UI::InGame_UI() : Alien()
{
}

InGame_UI::~InGame_UI()
{
}

void InGame_UI::Start()
{
	pause_menu->SetEnable(false);
	GameObject::FindWithName("Menu")->SetEnable(true);
	you_died = GameObject::FindWithName("YouDied");
	relics_panel = GameObject::FindWithName("Relics_Notification");
	relics_panel->SetEnable(false);
	you_died->SetEnable(false);
	in_game->SetEnable(true);
}

void InGame_UI::Update()
{
	if (((Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_START)) || (Input::GetControllerButtonUp(2, Input::CONTROLLER_BUTTON_START))||(Input::GetKeyDown(SDL_SCANCODE_ESCAPE)))&&!died)
	{
		PauseMenu(!Time::IsGamePaused());
	}

	if (died)
	{
		if (time + waiting < Time::GetGameTime())
		{
			if (!died_gone)
			{
				you_died->SetEnable(true);
				time = Time::GetGameTime();
				waiting = 2;
				died_gone = true;
			}
			else
			{
				SceneManager::LoadScene("NewWin_Menu");
			}
		}
	}
}

void InGame_UI::PauseMenu(bool to_open)
{
	in_game->SetEnable(!to_open);
	Time::SetPause(to_open);
	pause_menu->SetEnable(to_open);
	if (Time::IsGamePaused())
	{
		LOG("%s", "paused");
	}
	else
		LOG("%s", "going");
}

void InGame_UI::YouDied()
{
	died = true;
	time = Time::GetGameTime();
}