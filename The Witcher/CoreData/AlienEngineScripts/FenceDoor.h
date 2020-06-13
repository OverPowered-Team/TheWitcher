#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API FenceDoor : public Alien {

public:

	FenceDoor();
	virtual ~FenceDoor();
	
	void Start();

	void Explode();

	void Fall();

	void DeleteMyself();

	float force = 10.f;
	float time_to_despawn = 5.f;
};

ALIEN_FACTORY FenceDoor* CreateFenceDoor() {
	FenceDoor* alien = new FenceDoor();
	// To show in inspector here

	

	return alien;
} 
