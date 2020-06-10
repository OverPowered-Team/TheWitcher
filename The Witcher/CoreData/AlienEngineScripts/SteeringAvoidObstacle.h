#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
class Enemy;

class ALIEN_ENGINE_API SteeringAvoidObstacle : public Alien {

public:

	SteeringAvoidObstacle();
	virtual ~SteeringAvoidObstacle();
	
	void Start();
	void Update();

public:
	float distanceRay = 3.0f;
	Enemy* enemyScript = nullptr;
	std::vector<const char*> coll_layers;

};

ALIEN_FACTORY SteeringAvoidObstacle* CreateSteeringAvoidObstacle() {
	SteeringAvoidObstacle* alien = new SteeringAvoidObstacle();
	// To show in inspector here

	return alien;
} 
