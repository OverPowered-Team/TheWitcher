#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Wagonnette_Triggers.h"

enum(TRANSITION_STATE,
	FADE_IN,
	SHOWING,
	FADE_OUT,
	
	ANY);

class ALIEN_ENGINE_API Wagonnete_UI : public Alien {

public:

	Wagonnete_UI();
	virtual ~Wagonnete_UI();
	
	void Start();
	void Update();

public:

	void UpdateLifebar(float new_life, float max_life);

	void ActivateTrigger(Wagonnette_Triggers::TRIGGER_TYPE type);

	// TODO: pause (ui, dialogues)
	// xddddd NO

private:

	void HandleImage(ComponentImage* image);

public:

	GameObject* godmode = nullptr;
	float lerping_time = 1.f;

private:

	ComponentBar* lifebar = nullptr;

	ComponentImage* duck = nullptr;
	ComponentImage* dodge = nullptr;

	// States
	TRANSITION_STATE duck_state = TRANSITION_STATE::ANY;
	TRANSITION_STATE dodge_state = TRANSITION_STATE::ANY;

	// Lerping Life
	bool has_been_hit = false;
	bool changing_life = false;
	float life_lerp_time = 0.0f;
	float life_percentatge = 0.0f;
	float actual_life = 0.0f;

	// Images changing
	bool duck_changing = false;
	bool dodge_changing = false;
	float duck_time = 0.0f;
	float dodge_time = 0.0f;

};

ALIEN_FACTORY Wagonnete_UI* CreateWagonnete_UI() {
	Wagonnete_UI* alien = new Wagonnete_UI();
	// To show in inspector here

	return alien;
} 
