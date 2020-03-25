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
		PauseMenu(!pause_active);
	}
}

void InGame_UI::PauseMenu(bool to_open)
{
	in_game->SetEnable(!to_open);
	pause_menu->SetEnable(to_open);
	pause_active = to_open;
}