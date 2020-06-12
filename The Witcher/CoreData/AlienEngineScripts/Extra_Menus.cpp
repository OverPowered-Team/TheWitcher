#include "Extra_Menus.h"
#include "Stat.h"
#include "PlayerController.h"

Extra_Menus::Extra_Menus() : Alien()
{
}

Extra_Menus::~Extra_Menus()
{
}

void Extra_Menus::Start()
{
	// Menus
	leaderboard = game_object->GetChild("Leaderboard");
	controls = game_object->GetChild("Controls");
	B = game_object->GetChild("B");
	settings = game_object->GetChild("Settings");
	leaderboard->SetEnable(false);
	controls->SetEnable(false);
	B->SetEnable(false);
	settings->SetEnable(false);

	// Main Menu or InGame
	if (strcmp(SceneManager::GetCurrentScene(), "Main_Menu") == 0)
	{
		previous_menu = GameObject::FindWithName("Main_Menu_UI");
	}
	else
	{
		previous_menu = GameObject::FindWithName("Pause_Menu")->GetChild("Menu");
	}

	//ComponentCheckbox* checkbox = game_object->GetChild("Settings")->GetChild("CheckboxFS")->GetComponent<ComponentCheckbox>();
	// checkbox->SetStatus(true);

}

void Extra_Menus::PostUpdate()
{
	if ((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) 
		|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B)))
	{
		if ((strcmp(SceneManager::GetCurrentScene(), "Main_Menu")) == 0 || (Time::IsGamePaused()))
		{
			if (game_object->IsEnabled())
			{
				previous_menu->SetEnable(true);
				game_object->SetEnable(false);
			}
		}
	}
}

void Extra_Menus::MenuSpawn(MENU menu_type)
{
	switch (menu_type) 
	{
	case MENU::CONTROLS:
		leaderboard->SetEnable(false);
		controls->SetEnable(true);
		B->SetEnable(true);
		settings->SetEnable(false);
		break;
	case MENU::SETTINGS:
		leaderboard->SetEnable(false);
		controls->SetEnable(false);
		B->SetEnable(false);
		settings->SetEnable(false);
		break;
	case MENU::LEADERBOARD:
		leaderboard->SetEnable(true);
		controls->SetEnable(false);
		B->SetEnable(true);
		settings->SetEnable(false);
		break;
	}
}