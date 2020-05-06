#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API DrownedGrab : public Alien {

public:

	DrownedGrab();
	virtual ~DrownedGrab();
	
	void Start();
	void Update();

};

ALIEN_FACTORY DrownedGrab* CreateDrownedGrab() {
	DrownedGrab* alien = new DrownedGrab();
	// To show in inspector here

	return alien;
} 
