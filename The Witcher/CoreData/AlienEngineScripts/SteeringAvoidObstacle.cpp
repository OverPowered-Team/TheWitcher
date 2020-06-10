#include "SteeringAvoidObstacle.h"
#include "Enemy.h"

SteeringAvoidObstacle::SteeringAvoidObstacle() : Alien()
{
}

SteeringAvoidObstacle::~SteeringAvoidObstacle()
{
}

void SteeringAvoidObstacle::Start()
{
	enemyScript = game_object->GetComponent<Enemy>();
	coll_layers.push_back("Ground");
	coll_layers.push_back("Default");
}

void SteeringAvoidObstacle::Update()
{
	RaycastHit hit;

	/*if (Physics::Raycast(game_object->transform->GetGlobalPosition(), enemyScript->direction, distanceRay, hit, Physics::GetLayerMask(coll_layers)))
	{

	}*/
}