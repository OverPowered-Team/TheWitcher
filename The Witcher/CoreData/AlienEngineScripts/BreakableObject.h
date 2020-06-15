#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API BreakableObject : public Alien {

public:

	BreakableObject();
	virtual ~BreakableObject();
	
	void Explode();

	void Start() override; 

	void OnTriggerEnter(ComponentCollider* collider) override;

	Prefab object_broken;

	float force = 15.f;
	float time_to_despawn = 5.f;

	int hits_to_broke = 1;

private: 

	int current_hits = 0;
};

ALIEN_FACTORY BreakableObject* CreateBreakableObject() {
	BreakableObject* alien = new BreakableObject();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->object_broken);

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->hits_to_broke);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->force);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_despawn);

	return alien;
} 
