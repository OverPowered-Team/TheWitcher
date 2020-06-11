#include "FenceDoor.h"
#include "AttackTrigger.h"

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
	GameObject* broken = game_object->parent->GetChild(1);
	broken->SetEnable(true);
	if (broken != nullptr) {
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

	GameObject::Destroy(game_object);
}

void FenceDoor::Fall()
{
	auto rbs = game_object->parent->GetChild(1)->GetComponentsInChildren<ComponentConvexHullCollider>();
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

void FenceDoor::OnTriggerEnter(ComponentCollider* collider)
{
	if (collider->game_object_attached->IsEnabled())
	{
		if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
			++current_hits;
			if (current_hits >= hits_to_broke)
			{
				Explode();
				auto audio = collider->game_object_attached->GetComponent<AttackTrigger>()->player_obj->GetComponent<ComponentAudioEmitter>();
				//auto audio = collider->game_object_attached->parent->parent->parent->parent->parent->parent->parent->parent->parent->parent->parent->GetComponent<ComponentAudioEmitter>();
				if (audio)
					audio->StartSound("Play_FenceDestroy");
			}
		}
	}
}
