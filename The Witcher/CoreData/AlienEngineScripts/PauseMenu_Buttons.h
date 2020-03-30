#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PauseMenu_Buttons : public Alien {

public:

	PauseMenu_Buttons();
	virtual ~PauseMenu_Buttons();
	
	void Update();

	void Resume();
	void Leaderboard();
	void Controls();
	void Settings();
	void RetryLevel();
	void Exit_to_Menu();
	void Exit_Menu();

};

ALIEN_FACTORY PauseMenu_Buttons* CreatePauseMenu_Buttons() {
	PauseMenu_Buttons* alien = new PauseMenu_Buttons();
	// To show in inspector here

	SHOW_VOID_FUNCTION(PauseMenu_Buttons::Resume, alien);
	SHOW_VOID_FUNCTION(PauseMenu_Buttons::Leaderboard, alien);
	SHOW_VOID_FUNCTION(PauseMenu_Buttons::Controls, alien);

	return alien;
} 
