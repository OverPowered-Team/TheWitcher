#include "RockFall.h"

RockFall::RockFall() : Alien()
{
}

RockFall::~RockFall()
{
}

void RockFall::Start()
{
	spawnTime = Random::GetRandomFloatBetweenTwo(minSpawnTime, maxSpawnTime);
	currentTime = Time::GetGameTime();
}

void RockFall::Update()
{
	if (Time::GetGameTime() - currentTime > spawnTime)
	{
		float range = Random::GetRandomFloatBetweenTwo(-rangeSpawn, rangeSpawn);
		spawnTime = Random::GetRandomFloatBetweenTwo(minSpawnTime, maxSpawnTime);
		currentTime = Time::GetGameTime();
		ComponentRigidBody* rb = (ComponentRigidBody*)GameObject::Instantiate(rock, transform->GetGlobalPosition() + transform->forward * range)->GetComponent(ComponentType::RIGID_BODY);
		rb->AddForce(-transform->right * force);
	}
}