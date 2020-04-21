#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Scale_Wing : public Alien {

public:

	Scale_Wing();
	virtual ~Scale_Wing();
	
	void Start();
	void Update();

	// Triggers
	void OnTriggerEnter(ComponentCollider* collider) override;
};

ALIEN_FACTORY Scale_Wing* CreateScale_Wing() {
	Scale_Wing* alien = new Scale_Wing();
	// To show in inspector here

	return alien;
} 
