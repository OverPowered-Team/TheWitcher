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
	void ShowCheckpointSaved();

public:

	GameObject* in_game = nullptr;
	GameObject* pause_menu = nullptr;

private:

	bool died = false;
	bool died_gone = false;

	float time = 0.0f;
	int waiting = 1;

	GameObject* you_died = nullptr;
	GameObject* relics_panel = nullptr;
	bool pause_active = false;

	// Checkpoint Saved Text
	GameObject* checkpoint_saved_text = nullptr;
	float time_checkpoint = 0.0f;
};

ALIEN_FACTORY InGame_UI* CreateInGame_UI() {
	InGame_UI* alien = new InGame_UI();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->in_game);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->pause_menu);

	return alien;
} 
