#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RotateObject : public Alien {

public:

	RotateObject();
	virtual ~RotateObject();
	
	void Start();
	void Update();
	float yrotation;
	float speed = 0;
};

ALIEN_FACTORY RotateObject* CreateRotateObject() {
	RotateObject* alien = new RotateObject();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	return alien;
} 
