#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerTest : public Alien {

public:

	PlayerTest();
	virtual ~PlayerTest();
	
	void Start();
	void Update();

	float speed = 5.f;

};

ALIEN_FACTORY PlayerTest* CreatePlayerTest() {
	PlayerTest* alien = new PlayerTest();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speed);

	return alien;
} 
