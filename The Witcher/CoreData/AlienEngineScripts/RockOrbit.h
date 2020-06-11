#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockOrbit : public Alien {

public:

	RockOrbit();
	virtual ~RockOrbit();
	
	void Start();
	void Update();

	float init_time = 5.0f;
	float init_timer = 0.0f;
	float init_velocity = 0.01f;

	float3 self_rotation = { 0.0f, 5.0f, 2.0f };

	bool collided = false;

};

ALIEN_FACTORY RockOrbit* CreateRockOrbit() {
	RockOrbit* alien = new RockOrbit();
	// To show in inspector here

	return alien;
} 
