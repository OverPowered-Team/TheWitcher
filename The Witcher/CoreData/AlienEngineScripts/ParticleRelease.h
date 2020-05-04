#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ParticleRelease : public Alien {

public:

	ParticleRelease();
	virtual ~ParticleRelease();
	
	void Start();
	void Update();

	ComponentParticleSystem* p_system;

};

ALIEN_FACTORY ParticleRelease* CreateParticleRelease() {
	ParticleRelease* alien = new ParticleRelease();
	// To show in inspector here

	return alien;
} 
