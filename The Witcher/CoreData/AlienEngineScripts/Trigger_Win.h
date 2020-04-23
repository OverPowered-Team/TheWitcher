#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Trigger_Win : public Alien {

public:

	Trigger_Win();
	virtual ~Trigger_Win();
	
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);

	bool first = false;
};

ALIEN_FACTORY Trigger_Win* CreateTrigger_Win() {
	Trigger_Win* alien = new Trigger_Win();
	// To show in inspector here

	return alien;
} 
