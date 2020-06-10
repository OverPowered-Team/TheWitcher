#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Credits : public Alien {

public:

	Credits();
	virtual ~Credits();
	
	void Start();
	void Update();

	float time_credits_last = 40.0f;
	float last_position = 400.0f;
	float time = 0.0f;

	ComponentText* points1 = nullptr;
	ComponentText* points2 = nullptr;
};

ALIEN_FACTORY Credits* CreateCredits() {
	Credits* alien = new Credits();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_credits_last);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->last_position);

	return alien;
} 
