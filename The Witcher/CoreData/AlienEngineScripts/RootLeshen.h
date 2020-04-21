#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RootLeshen : public Alien {

public:

	RootLeshen();
	virtual ~RootLeshen();
	

	float speed, life_time, root_time, total_life_time;
	float3 direction;
	int target = 0;

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);

};

ALIEN_FACTORY RootLeshen* CreateRootLeshen() {
	RootLeshen* alien = new RootLeshen();
	// To show in inspector here

	return alien;
} 
