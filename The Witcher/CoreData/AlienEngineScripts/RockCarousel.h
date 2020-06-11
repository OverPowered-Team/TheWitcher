#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockCarousel : public Alien {

public:

	RockCarousel();
	virtual ~RockCarousel();
	
	void Start();
	void Update();

};

ALIEN_FACTORY RockCarousel* CreateRockCarousel() {
	RockCarousel* alien = new RockCarousel();
	// To show in inspector here

	return alien;
} 
