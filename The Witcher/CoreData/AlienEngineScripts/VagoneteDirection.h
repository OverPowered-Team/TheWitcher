#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API VagoneteDirection : public Alien {

public:

	VagoneteDirection();
	virtual ~VagoneteDirection();

	bool default_right = true;
	GameObject* curve_left = nullptr;
	GameObject* curve_right = nullptr;
	float velocity = 0.1F;
};

ALIEN_FACTORY VagoneteDirection* CreateVagoneteDirection() {
	VagoneteDirection* alien = new VagoneteDirection();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->default_right);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->curve_right);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->curve_left);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->velocity);
	return alien;
} 
