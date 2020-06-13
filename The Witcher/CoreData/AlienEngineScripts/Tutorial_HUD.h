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

	void ShowTriggerAttack(bool show);
	void ShowTriggerDash(bool show);
	void ShowTriggerMagic(bool show);
	void ShowTriggerRelics(bool show);

private:
	
	void Start();
	void Update();

	void HandleTriggerDash();
	void HandleTriggerMagic();
	void HandleTriggerAttack();
	void HandleTriggerRelic();

private:

	// States
	Current_Showing current_state_attack = Current_Showing::ANY;
	Current_Showing current_state_dash = Current_Showing::ANY;
	Current_Showing current_state_magic = Current_Showing::ANY;
	Current_Showing current_state_relics = Current_Showing::ANY;

	// Components Attack
	ComponentImage*		X			= nullptr;
	ComponentImage*		Y			= nullptr;
	ComponentText*		text_attack	= nullptr;

	// Components Dash
	ComponentImage*		RB			= nullptr;
	ComponentText*		text_dash	= nullptr;

	// Components Magic
	std::vector<ComponentImage*> magic_images;
	ComponentText* text_magic = nullptr;

	// Components Relics
	ComponentImage* back_relic = nullptr;
	ComponentText* text_relic = nullptr;

	// Effects
	bool show_attack	= false;
	bool show_dash		= false;
	bool show_magic		= false;
	bool show_relics	= false;
	float attack_time	= 0.0f;
	float dash_time		= 0.0f;
	float magic_time	= 0.0f;
	float relics_time	= 0.0f;

};

ALIEN_FACTORY Tutorial_HUD* CreateTutorial_HUD() {
	Tutorial_HUD* alien = new Tutorial_HUD();
	// To show in inspector here

	return alien;
} 
