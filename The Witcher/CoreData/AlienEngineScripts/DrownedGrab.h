#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;

class ALIEN_ENGINE_API DrownedGrab : public Drowned {

public:

	DrownedGrab();
	virtual ~DrownedGrab();
	
	void UpdateEnemy() override;

};

ALIEN_FACTORY DrownedGrab* CreateDrownedGrab() {
	DrownedGrab* alien = new DrownedGrab();
	// To show in inspector here

	return alien;
} 
