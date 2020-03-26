#include "Extra_Menus.h"

Extra_Menus::Extra_Menus() : Alien()
{
}

Extra_Menus::~Extra_Menus()
{
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
		GameObject::FindWithName("Settings")->SetEnable(false);
		GameObject::FindWithName("Controls")->SetEnable(true);
		break;
	case MENU::SETTINGS:
		GameObject::FindWithName("Controls")->SetEnable(false);
		GameObject::FindWithName("Settings")->SetEnable(true);
		break;
	}
}