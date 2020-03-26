#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Extra_Menus : public Alien {

public:

	enum(MENU,
		CONTROLS,
		SETTINGS,
		);

	Extra_Menus();
	virtual ~Extra_Menus();

	void MenuSpawn(MENU menu_type);

	GameObject* extramenus_canvas = nullptr;

	// Buttons


};

ALIEN_FACTORY Extra_Menus* CreateExtra_Menus() {
	Extra_Menus* alien = new Extra_Menus();
	// To show in inspector here

	SHOW_LAMBDA_FUNCTION(alien->MenuSpawn,"MenuControls",alien,Extra_Menus::MENU::CONTROLS);
	SHOW_LAMBDA_FUNCTION(alien->MenuSpawn,"MenuSettings",alien,Extra_Menus::MENU::SETTINGS);

	return alien;
} 
