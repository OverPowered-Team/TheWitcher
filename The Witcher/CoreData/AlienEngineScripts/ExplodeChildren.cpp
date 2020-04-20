#include "ExplodeChildren.h"

ExplodeChildren::ExplodeChildren() : Alien()
{
}

ExplodeChildren::~ExplodeChildren()
{
}

void ExplodeChildren::Start()
{
	auto rbs = game_object->GetComponentsInChildren<ComponentRigidBody>();
	for (auto i = rbs.begin(); i != rbs.end(); ++i) {
		(*i)->AddForce(float3(Random::GetRandomIntBetweenTwo(-50, 50), Random::GetRandomIntBetweenTwo(0, 30), Random::GetRandomIntBetweenTwo(-50, 50)).Normalized() * 3.0);
	}

	Invoke(std::bind(&ExplodeChildren::DeleteMyself, this), 4);
}

void ExplodeChildren::Update()
{
}

void ExplodeChildren::DeleteMyself()
{
	GameObject::Destroy(game_object);
}
