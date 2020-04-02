#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API InGame_UI : public Alien {

public:

	InGame_UI();
	virtual ~InGame_UI();
	
	void Start();
	void Update();

	void PauseMenu(bool to_open);
	void YouDied();

	bool died = false;
	bool died_gone = false;

	float time = 0.0;
	int waiting = 1;

	GameObject* in_game = nullptr;
	GameObject* pause_menu = nullptr;
	GameObject* you_died = nullptr;
	bool pause_active = false;
};

ALIEN_FACTORY InGame_UI* CreateInGame_UI() {
	InGame_UI* alien = new InGame_UI();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->in_game);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->pause_menu);

	return alien;
} 
