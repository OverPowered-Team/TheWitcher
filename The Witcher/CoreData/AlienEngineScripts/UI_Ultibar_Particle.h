#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UI_Ultibar_Particle : public Alien {

public:

	UI_Ultibar_Particle();
	virtual ~UI_Ultibar_Particle();
	
	void Start();
	void Update();

	float particle_time = 0.0f;
	int sign = 1;
	float original_scale = 0.0f;
};

ALIEN_FACTORY UI_Ultibar_Particle* CreateUI_Ultibar_Particle() {
	UI_Ultibar_Particle* alien = new UI_Ultibar_Particle();
	// To show in inspector here

	return alien;
} 
