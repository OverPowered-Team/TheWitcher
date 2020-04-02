#include "BreakableObject.h"

BreakableObject::BreakableObject() : Alien()
{
}

BreakableObject::~BreakableObject()
{
}

void BreakableObject::Start()
{
}

void BreakableObject::Update()
{
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
		GameObject::Destroy(game_object);
	}
}
