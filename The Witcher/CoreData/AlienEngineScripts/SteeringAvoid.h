#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;

class ALIEN_ENGINE_API SteeringAvoid : public Alien {

public:

	SteeringAvoid();
	virtual ~SteeringAvoid();
	
	void Start();
	float3 AvoidObstacle(float& a_force);

	void OnDrawGizmosSelected();

public:
	float avoidRange = 3.0f;
	Enemy* enemyScript = nullptr;

};

ALIEN_FACTORY SteeringAvoid* CreateSteeringAvoid() {
	SteeringAvoid* steeringAvoid = new SteeringAvoid();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(steeringAvoid->avoidRange);

	return steeringAvoid;
} 
