#include "PauseMenu_Buttons.h"
#include "InGame_UI.h"

PauseMenu_Buttons::PauseMenu_Buttons() : Alien()
{
}

PauseMenu_Buttons::~PauseMenu_Buttons()
{
}

void PauseMenu_Buttons::Update()
{
	if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) || Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B))
	{
		((InGame_UI*)GameObject::FindWithName("UI_InGame")->GetComponentScript("InGame_UI"))->PauseMenu(false);
	}
}

void PauseMenu_Buttons::Resume()
{
	((InGame_UI*)GameObject::FindWithName("UI_InGame")->GetComponentScript("InGame_UI"))->PauseMenu(false);
}

void PauseMenu_Buttons::Leaderboard()
{
}

void PauseMenu_Buttons::Controls()
{
}

void PauseMenu_Buttons::Settings()
{
}

void PauseMenu_Buttons::RetryLevel()
{
}

void PauseMenu_Buttons::Exit_to_Menu()
{
}

void PauseMenu_Buttons::Exit_Menu()
{
}

