#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Floating : public Alien {

public:

	Floating();
	virtual ~Floating();
	
	void Start();
	void Update();

	float time = 0.0f;
	float time_r = 0.0f;
	bool random_start = false;
	int sign = 1;
	float height = 0.5f;
	float cicle_time = 2.0f;
	bool random = false;
};

ALIEN_FACTORY Floating* CreateFloating() {
	Floating* alien = new Floating();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->height);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->cicle_time);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->random);
	return alien;
} 
