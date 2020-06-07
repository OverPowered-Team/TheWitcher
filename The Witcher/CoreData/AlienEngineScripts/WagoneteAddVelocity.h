#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API WagoneteAddVelocity : public Alien {

public:

	WagoneteAddVelocity();
	virtual ~WagoneteAddVelocity();
	
	void Start();
	void Update();


	float max_velocity = 0.0F;
	float acceleration = 0.0F;
};

ALIEN_FACTORY WagoneteAddVelocity* CreateWagoneteAddVelocity() {
	WagoneteAddVelocity* alien = new WagoneteAddVelocity();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->max_velocity);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->acceleration);
	return alien;
} 
