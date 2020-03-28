#include "ArrowMovement.h"

ArrowMovement::ArrowMovement() : Alien()
{
}

ArrowMovement::~ArrowMovement()
{
}

void ArrowMovement::Start()
{
	rb = (ComponentRigidBody*)game_object->GetComponent(ComponentType::RIGID_BODY);
}

void ArrowMovement::Update()
{
	//rb->AddForce(direction.Mul(1));
}