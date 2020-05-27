#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockThrow : public Alien {

public:
	float lifetime = 10.0f;
	float timer = 0.0f;

	RockThrow();
	virtual ~RockThrow();
	
	void Start();
	void Update();
	void ReleaseExplosionParticle();
	void OnTriggerEnter(ComponentCollider* collider);

	GameObject* particle_instance = nullptr;

};

ALIEN_FACTORY RockThrow* CreateRockThrow() {
	RockThrow* alien = new RockThrow();
	// To show in inspector here

	return alien;
} 
