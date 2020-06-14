#include "FenceDoor.h"

FenceDoor::FenceDoor() : Alien()
{
}

FenceDoor::~FenceDoor()
{
}

void FenceDoor::Start()
{
	GameObject* objparticles = game_object->GetChild("Particles");
	if (objparticles != nullptr) {
		std::vector<ComponentParticleSystem*> particle_gos = objparticles->GetComponentsInChildren<ComponentParticleSystem>();
		for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
			particles.insert(std::pair((*it)->game_object_attached->GetName(), (*it)));
			(*it)->OnStop();
		}
	}
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

		if (particles.find("explosion") != particles.end())
			particles["explosion"]->Restart();
		if (particles.find("smoke") != particles.end())
			particles["smoke"]->Restart();
	}

	GameObject::Destroy(game_object->GetChild(0));
}

void FenceDoor::Fall()
{
	auto rbs = game_object->GetChild(0)->GetComponentsInChildren<ComponentConvexHullCollider>();
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
