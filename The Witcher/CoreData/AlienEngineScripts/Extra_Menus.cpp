#include "Extra_Menus.h"

Extra_Menus::Extra_Menus() : Alien()
{
}

Extra_Menus::~Extra_Menus()
{
}

void Extra_Menus::PostUpdate()
{
	if ((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) 
		|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B)))
	{
		if (game_object->IsEnabled())
		{
			previous_menu->SetEnable(true);
				game_object->SetEnable(false);
		}
	}
}

void Extra_Menus::MenuSpawn(MENU menu_type)
{
	switch (menu_type) 
	{
	case MENU::CONTROLS:
		GameObject::FindWithName("Leaderboard")->SetEnable(false);
		GameObject::FindWithName("Settings")->SetEnable(false);
		GameObject::FindWithName("Controls")->SetEnable(true);
		break;
	case MENU::SETTINGS:
		GameObject::FindWithName("Leaderboard")->SetEnable(false);
		GameObject::FindWithName("Controls")->SetEnable(false);
		GameObject::FindWithName("Settings")->SetEnable(true);
		break;
	case MENU::LEADERBOARD:
		GameObject::FindWithName("Controls")->SetEnable(false);
		GameObject::FindWithName("Settings")->SetEnable(false);
		GameObject::FindWithName("Leaderboard")->SetEnable(true);
		break;
	}
}