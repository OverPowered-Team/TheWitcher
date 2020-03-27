#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UI_EndMenu : public Alien {

public:

	UI_EndMenu();
	virtual ~UI_EndMenu();
	
	enum(ENDMENU,
		DAMAGE,
		KILLS,
		RELICS,
		GOLD,
		XP,
		);

	bool first_time = false;

	void EndMenu(ENDMENU menu);
};

ALIEN_FACTORY UI_EndMenu* CreateUI_EndMenu() {
	UI_EndMenu* alien = new UI_EndMenu();
	// To show in inspector here

	SHOW_LAMBDA_FUNCTION(alien->EndMenu, "Damage", alien, UI_EndMenu::ENDMENU::DAMAGE);
	SHOW_LAMBDA_FUNCTION(alien->EndMenu, "Kills", alien, UI_EndMenu::ENDMENU::KILLS);
	SHOW_LAMBDA_FUNCTION(alien->EndMenu, "Relics", alien, UI_EndMenu::ENDMENU::RELICS);
	SHOW_LAMBDA_FUNCTION(alien->EndMenu, "Gold", alien, UI_EndMenu::ENDMENU::GOLD);
	SHOW_LAMBDA_FUNCTION(alien->EndMenu, "XP", alien, UI_EndMenu::ENDMENU::XP);

	return alien;
} 
