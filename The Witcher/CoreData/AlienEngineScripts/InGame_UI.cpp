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
	in_game->SetEnable(true);
}

void InGame_UI::Update()
{
	if ((Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_START)) || (Input::GetControllerButtonUp(2, Input::CONTROLLER_BUTTON_START)))
	{
		if (!pause_active)
		{
			// Pause Game
			in_game->SetEnable(false);
			pause_menu->SetEnable(true);
			pause_active = true;
		}
		else
		{
			// Set original time speed
			pause_menu->SetEnable(false);
			in_game->SetEnable(true);
			pause_active = false;
		}
	}
}