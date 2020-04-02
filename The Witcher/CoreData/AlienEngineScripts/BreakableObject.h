#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API BreakableObject : public Alien {

public:

	BreakableObject();
	virtual ~BreakableObject();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;

	Prefab object_broken;
};

ALIEN_FACTORY BreakableObject* CreateBreakableObject() {
	BreakableObject* alien = new BreakableObject();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->object_broken);

	return alien;
} 
