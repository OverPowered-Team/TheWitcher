#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Leshen;

class ALIEN_ENGINE_API CrowsLeshen : public Alien {

public:
	float speed = 0.2f;
	float life_time = 0.0f;
	float total_life_time = 3.0f;
	float3 direction = {1, 0, 1};
	int target = 0;
	bool setted_direction;
	Leshen* leshen = nullptr;

	CrowsLeshen();
	virtual ~CrowsLeshen();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);
};

ALIEN_FACTORY CrowsLeshen* CreateCrowsLeshen() {
	CrowsLeshen* alien = new CrowsLeshen();
	// To show in inspector here

	return alien;
} 
