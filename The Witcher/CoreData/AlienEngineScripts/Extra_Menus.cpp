#include "Extra_Menus.h"

Extra_Menus::Extra_Menus() : Alien()
{
}

Extra_Menus::~Extra_Menus()
{
}

void Extra_Menus::Update()
{
	if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) || Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B))
	{
		GameObject::FindWithName("Extra_Menus")->SetEnable(false);
	}
}

void Extra_Menus::MenuSpawn(MENU menu_type)
{
	extramenus_canvas = GameObject::FindWithName("Extra_Menus");
	if (!extramenus_canvas)
	{
		// CREATE EXTRA_MENUS PREFAB
		// AND MOVE IT ON TOP (NEW RELEASE AND FEATURE REQUEST #87 DEPENDENT)
	}
	if (!extramenus_canvas->IsEnabled())
	{
		extramenus_canvas->SetEnable(true);
	}

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