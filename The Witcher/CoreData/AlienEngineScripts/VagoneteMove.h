#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API VagoneteMove : public Alien {

public:

	VagoneteMove() {}
	virtual ~VagoneteMove() {}
	
	void Start();
	void Update();

public:
	float actual_pos = 0.0F;
	ComponentCurve* curve = nullptr;

	float speed = 1;

	float inclination4player = 20.0F;
	float speedInclination = 5.0F;

private:

	float currentInclination1 = 0.0F;
	float currentInclination2 = 0.0F;

};

ALIEN_FACTORY VagoneteMove* CreateVagoneteMove() {
	VagoneteMove* alien = new VagoneteMove();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speedInclination);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->inclination4player);
	return alien;
} 
