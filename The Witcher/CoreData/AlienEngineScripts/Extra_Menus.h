#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Extra_Menus : public Alien {

public:

	enum(MENU,
		CONTROLS,
		SETTINGS,
		LEADERBOARD,
		);

	Extra_Menus();
	virtual ~Extra_Menus();

	void Start();
	void PostUpdate();

	void MenuSpawn(MENU menu_type);

	GameObject* extramenus_canvas = nullptr;
	GameObject* previous_menu = nullptr;
};

ALIEN_FACTORY Extra_Menus* CreateExtra_Menus() {
	Extra_Menus* alien = new Extra_Menus();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->previous_menu);

	return alien;
} 
