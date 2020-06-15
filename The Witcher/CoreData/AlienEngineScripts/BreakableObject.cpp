#include "BreakableObject.h"
#include "ExplodeChildren.h"
#include "AttackTrigger.h"

BreakableObject::BreakableObject() : Alien()
{
	
}

BreakableObject::~BreakableObject()
{
}

void BreakableObject::Explode()
{
	GameObject* new_obj = GameObject::Instantiate(object_broken, transform->GetGlobalPosition()/*, false, game_object->parent*/);
	if (new_obj != nullptr) {
		auto c = new_obj->GetComponent<ExplodeChildren>();
		if (c != nullptr)
			c->SetVars(force, time_to_despawn);

		auto children = new_obj->GetComponentsInChildren<ComponentRigidBody>();
		for (auto i = children.begin(); i != children.end(); ++i) {
			(*i)->SetPosition((*i)->game_object_attached->transform->GetGlobalPosition());
		}

		new_obj->GetComponent<ExplodeChildren>()->Explode();
	}

	GameObject::Destroy(game_object);
}

void BreakableObject::Start()
{
	ComponentCollider* collider = game_object->GetComponent<ComponentCollider>();
	if (collider != nullptr)
	{
		collider->SetCollisionLayer("Default");
	}
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
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
				if(audio)
					audio->StartSound("Play_FenceDestroy");
			}
		}
	}
}
