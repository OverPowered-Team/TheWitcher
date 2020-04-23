#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockDownHill : public Alien {

public:

	RockDownHill();
	virtual ~RockDownHill();
	
	void Start();
	void Update();

	void CalculateDirection(const float3& end_pos);
	void SetMoveAndRotationSpeed(const float& rot_speed, const float& speed);

	void OnTriggerEnter(ComponentCollider* trigger) override;

	float3 direction = float3::zero();

	float damage = 1.f;
private:
	float3 axis_rot = float3::zero();
	ComponentRigidBody* rb = nullptr;
	float rot_speed = 10.f;
	float speed = 5.f;
};

ALIEN_FACTORY RockDownHill* CreateRockDownHill() {
	RockDownHill* alien = new RockDownHill();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->damage);

	return alien;
} 
