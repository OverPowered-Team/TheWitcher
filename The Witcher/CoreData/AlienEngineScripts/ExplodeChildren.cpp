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
		rbs[i]->AddForce(float3(Random::GetRandomIntBetweenTwo(-50, 50), Random::GetRandomIntBetweenTwo(0, 30), Random::GetRandomIntBetweenTwo(-50, 50)).Normalized() * 10);
	}

	GameObject::FreeArrayMemory((void***)&rbs);
	Invoke(std::bind(&ExplodeChildren::DeleteMyself, this), 4);
}

void ExplodeChildren::Update()
{
}

void ExplodeChildren::DeleteMyself()
{
	GameObject::Destroy(game_object);
}
