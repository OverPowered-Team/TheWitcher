#include "ExplodeChildren.h"

ExplodeChildren::ExplodeChildren() : Alien()
{
}

ExplodeChildren::~ExplodeChildren()
{
}

void ExplodeChildren::Start()
{
	ComponentRigidBody** rbs = nullptr;

	uint size = game_object->GetComponentsInChildren(ComponentType::RIGID_BODY, (Component***)&rbs, false);
	for (auto i = 0u; i < size; ++i) {
		rbs[i]->AddForce(float3(Random::GetRandomIntBetweenTwo(10, 100), Random::GetRandomIntBetweenTwo(10, 100), Random::GetRandomIntBetweenTwo(10, 100)).Normalized()*10);
	}

	GameObject::FreeArrayMemory((void***)&rbs);
}

void ExplodeChildren::Update()
{
}