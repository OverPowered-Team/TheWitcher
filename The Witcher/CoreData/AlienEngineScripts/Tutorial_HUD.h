#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(Current_Showing,
	FADING_IN,
	SHOWING,
	FADING_OUT,

	ANY
	);

class ALIEN_ENGINE_API Tutorial_HUD : public Alien {

public:

	Tutorial_HUD();
	virtual ~Tutorial_HUD();
	
public:

	void ShowTriggerAttack();
	void ShowTriggerDash();

private:
	
	void Start();
	void Update();

private:

	// States
	Current_Showing current_state_attack = Current_Showing::ANY;
	Current_Showing current_state_dash = Current_Showing::ANY;

	// Components
	ComponentImage*		X		= nullptr;
	ComponentImage*		Y		= nullptr;
	ComponentText*		text	= nullptr;

	// Effects
	bool show_attack	= false;
	bool show_dash		= false;

};

ALIEN_FACTORY Tutorial_HUD* CreateTutorial_HUD() {
	Tutorial_HUD* alien = new Tutorial_HUD();
	// To show in inspector here

	return alien;
} 
