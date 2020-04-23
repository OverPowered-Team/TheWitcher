#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API BreakableObject : public Alien {

public:

	BreakableObject();
	virtual ~BreakableObject();
	
	void Start();
	void Explode();

	void OnTriggerEnter(ComponentCollider* collider) override;

	Prefab object_broken;

	float force = 15.f;
	float time_to_despawn = 5.f;
};

ALIEN_FACTORY BreakableObject* CreateBreakableObject() {
	BreakableObject* alien = new BreakableObject();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->object_broken);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->force);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_despawn);

	return alien;
} 
