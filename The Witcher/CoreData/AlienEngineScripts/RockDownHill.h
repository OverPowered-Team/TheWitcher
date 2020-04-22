#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockDownHill : public Alien {

public:

	RockDownHill();
	virtual ~RockDownHill();
	
	void Start();
	void Update();

	void CalculateDirection();

	void OnTriggerEnter(ComponentCollider* trigger) override;

	float3 end_pos = float3::zero();
	float3 direction = float3::zero();
	float speed = 5.f;

	ComponentRigidBody* rb = nullptr;
};

ALIEN_FACTORY RockDownHill* CreateRockDownHill() {
	RockDownHill* alien = new RockDownHill();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);

	return alien;
} 
