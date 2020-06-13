#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API BreakDoor : public Alien {

public:

	BreakDoor();
	virtual ~BreakDoor();

	void OnTriggerEnter(ComponentCollider* collider);

	float force = 10.f;
	float time_to_despawn = 5.f;

	int hits_to_broke = 4;

private:

	int current_hits = 0;
};

ALIEN_FACTORY BreakDoor* CreateBreakDoor() {
	BreakDoor* alien = new BreakDoor();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->hits_to_broke);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->force);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_despawn);

	return alien;
} 
