#include "FenceDoor.h"

FenceDoor::FenceDoor() : Alien()
{
}

FenceDoor::~FenceDoor()
{
}

void FenceDoor::Start()
{
}

void FenceDoor::Explode()
{
	GameObject* broken = game_object->GetChild(1);
	if (broken != nullptr) {
		broken->SetEnable(true);
		auto rbs = broken->GetComponentsInChildren<ComponentRigidBody>();
		for (auto i = rbs.begin(); i != rbs.end(); ++i) {
			(*i)->SetIsKinematic(false);
			(*i)->AddForce(float3(Random::GetRandomFloatBetweenTwo(-1.f, 1.f), Random::GetRandomFloatBetweenTwo(0, 1.f), Random::GetRandomFloatBetweenTwo(-1.f, 1.f)).Normalized() * force);
		}

		Invoke(std::bind(&FenceDoor::Fall, this), time_to_despawn);

		/*if (particles.find("explosion") != particles.end())
			particles["explosion"]->Restart();
		if (particles.find("smoke") != particles.end())
			particles["smoke"]->Restart();*/
	}

	game_object->GetChild(0)->GetComponent<ComponentRigidBody>()->Destroy();
	game_object->GetChild(0)->GetComponent<ComponentBoxCollider>()->Destroy();
	game_object->GetChild(0)->SetEnable(false);
}

void FenceDoor::Fall()
{
	auto rbs = game_object->GetChild(1)->GetComponentsInChildren<ComponentConvexHullCollider>();
	for (auto i = rbs.begin(); i != rbs.end(); ++i) {
		(*i)->SetEnable(false);
		(*i)->Destroy();
	}
	Invoke(std::bind(&FenceDoor::DeleteMyself, this), 2.f);
}

void FenceDoor::DeleteMyself()
{
	GameObject::Destroy(game_object);
}
