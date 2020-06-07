#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Leshen;

class ALIEN_ENGINE_API RootLeshen : public Alien {

public:
	enum(ROOTSTATE,
		NONE = -1,
		SEEK,
		ROOT
		);

	RootLeshen();
	virtual ~RootLeshen();
	void Reset();
	

	float speed = 0.08f;
	float root_speed = 0.005f;
	float3 rooted_effect_direction = {0.0f, 1.0f, 0.0f};
	float life_time = 0.0f;
	float root_time = 0.0f;
	float total_life_time = 5.0f;
	float total_root_time = 0.0f;
	float root_time_distance_factor = 0.08f;
	float base_total_root_time = 2.0f;
	float3 direction;

	ComponentRigidBody* rbody = nullptr;

	int target = 0;

	bool active = false;

	Leshen* leshen = nullptr;

	ROOTSTATE state = ROOTSTATE::NONE;

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);

};

ALIEN_FACTORY RootLeshen* CreateRootLeshen() {
	RootLeshen* alien = new RootLeshen();
	// To show in inspector here

	return alien;
} 
