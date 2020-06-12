#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MainMenu_Buttons : public Alien {

public:
	bool fullscreen = false;
	float volume = 1.0f;

	MainMenu_Buttons();
	virtual ~MainMenu_Buttons();

	void NewGame();
	void ExitGame();
	void Controls();
	void Credits();
	void Muffin();

	void AddVolume();
	void SubtractVolume();

	void FullScreen();

};

ALIEN_FACTORY MainMenu_Buttons* CreateMainMenu_Buttons() {
	MainMenu_Buttons* alien = new MainMenu_Buttons();
	// To show in inspector here

	SHOW_VOID_FUNCTION(MainMenu_Buttons::Muffin, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::NewGame, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::ExitGame, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::Controls, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::Credits, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::AddVolume, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::SubtractVolume, alien);
	SHOW_VOID_FUNCTION(MainMenu_Buttons::FullScreen, alien);

	return alien;
} 
