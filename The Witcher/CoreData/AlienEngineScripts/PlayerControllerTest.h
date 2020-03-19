#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerControllerTest : public Alien {

public:

	PlayerControllerTest();
	virtual ~PlayerControllerTest();
	
	void Start();
	void Update();

	int controllerIndex = 1;
	float movementSpeed = 5.f;
	float jumpPower = 50.f;
	ComponentCharacterController * ccontroller = nullptr;
};

ALIEN_FACTORY PlayerControllerTest* CreatePlayerControllerTest() {
	PlayerControllerTest* alien = new PlayerControllerTest();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->jumpPower);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->movementSpeed);
	return alien;
} 
