#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CloudLeshen : public Alien {

public:

	CloudLeshen();
	virtual ~CloudLeshen();
	
	void Start();
	void Update();

	float cloud_damage = 4.0f;
	float cloud_damage_tick = 1.0f;
	float cloud_damage_tick_timer = 1.0f;
	bool can_damage = false;

	ComponentSphereCollider* col = nullptr;

	bool player_entered[2];

	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);

};

ALIEN_FACTORY CloudLeshen* CreateCloudLeshen() {
	CloudLeshen* alien = new CloudLeshen();
	// To show in inspector here

	return alien;
} 
