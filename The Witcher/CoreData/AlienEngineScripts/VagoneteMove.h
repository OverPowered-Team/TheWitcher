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

};

ALIEN_FACTORY VagoneteMove* CreateVagoneteMove() {
	VagoneteMove* alien = new VagoneteMove();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	return alien;
} 
