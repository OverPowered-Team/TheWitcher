#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RootLeshen : public Alien {

public:

	RootLeshen();
	virtual ~RootLeshen();
	

	float speed = 0.1f;
	float life_time = 0.0f;
	float root_time = 5.0f;
	float total_life_time = 10.0f;
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
