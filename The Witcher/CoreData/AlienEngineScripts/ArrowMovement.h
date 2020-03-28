#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ArrowMovement : public Alien {

public:

	ArrowMovement();
	virtual ~ArrowMovement();
	
	void Start();
	void Update();

public:
	ComponentRigidBody* rb = nullptr;
	float3 direction;

};

ALIEN_FACTORY ArrowMovement* CreateArrowMovement() {
	ArrowMovement* alien = new ArrowMovement();
	// To show in inspector here

	return alien;
} 
