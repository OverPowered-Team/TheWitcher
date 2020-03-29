#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Win_lose_menu : public Alien {

public:

	Win_lose_menu();
	virtual ~Win_lose_menu();
	
	void Start();

	bool have_won = true;

	GameObject* win_panel = nullptr;
	GameObject* defeat_panel = nullptr;
	GameObject* win_icon = nullptr;
	GameObject* defeat_icon = nullptr;

};

ALIEN_FACTORY Win_lose_menu* CreateWin_lose_menu() {
	Win_lose_menu* alien = new Win_lose_menu();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->win_panel);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->defeat_panel);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->win_icon);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->defeat_icon);

	return alien;
} 
