#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API DummyEnemy : public Alien {

public:

	DummyEnemy();
	virtual ~DummyEnemy();
	
	void Start();
	void Update();

};

ALIEN_FACTORY DummyEnemy* CreateDummyEnemy() {
	DummyEnemy* alien = new DummyEnemy();
	// To show in inspector here

	return alien;
} 
