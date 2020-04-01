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
	in_game->SetEnable(true);
}

void InGame_UI::Update()
{
	if ((Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_START)) || (Input::GetControllerButtonUp(2, Input::CONTROLLER_BUTTON_START))||(Input::GetKeyDown(SDL_SCANCODE_ESCAPE)))
	{
		PauseMenu(!Time::IsGamePaused());
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