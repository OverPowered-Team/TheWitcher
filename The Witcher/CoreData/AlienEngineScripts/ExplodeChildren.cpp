#include "ExplodeChildren.h"

ExplodeChildren::ExplodeChildren() : Alien()
{
}

ExplodeChildren::~ExplodeChildren()
{
}

void ExplodeChildren::Start()
{
	has_broke = true;
	GameObject* objparticles = game_object->GetChild("Particles");
	if (objparticles != nullptr) {
		std::vector<ComponentParticleSystem*> particle_gos = objparticles->GetComponentsInChildren<ComponentParticleSystem>();
		for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
			particles.insert(std::pair((*it)->game_object_attached->GetName(), (*it)));
			(*it)->OnStop();
		}
	}
}

void ExplodeChildren::Explode()
{
	auto rbs = game_object->GetComponentsInChildren<ComponentRigidBody>();
	for (auto i = rbs.begin(); i != rbs.end(); ++i) {
		(*i)->AddForce(float3(Random::GetRandomFloatBetweenTwo(-1.f, 1.f), Random::GetRandomFloatBetweenTwo(0, 1.f), Random::GetRandomFloatBetweenTwo(-1.f, 1.f)).Normalized() * force);
	}

	Invoke(std::bind(&ExplodeChildren::Fall, this), time_to_disappear);

	if (particles.find("explosion") != particles.end())
		particles["explosion"]->Restart();
	if (particles.find("smoke") != particles.end())
		particles["smoke"]->Restart();
}

void ExplodeChildren::SetVars(float force, float time_despawn)
{
	this->force = force;
	time_to_disappear = time_despawn;
}

void ExplodeChildren::Fall()
{
	auto rbs = game_object->GetComponentsInChildren<ComponentConvexHullCollider>();
	for (auto i = rbs.begin(); i != rbs.end(); ++i) {
		(*i)->SetEnable(false);
		(*i)->Destroy();
	}
	Invoke(std::bind(&ExplodeChildren::DeleteMyself, this), 2.f);
}

void ExplodeChildren::DeleteMyself()
{
	GameObject::Destroy(game_object);
}
