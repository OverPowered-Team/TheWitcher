#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Tutorial_HUD : public Alien {

public:

	Tutorial_HUD();
	virtual ~Tutorial_HUD();
	
	void Start();
	void Update();

};

ALIEN_FACTORY Tutorial_HUD* CreateTutorial_HUD() {
	Tutorial_HUD* alien = new Tutorial_HUD();
	// To show in inspector here

	return alien;
} 
