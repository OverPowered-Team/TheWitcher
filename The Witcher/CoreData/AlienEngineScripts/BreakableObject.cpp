#include "BreakableObject.h"
#include "ExplodeChildren.h"

BreakableObject::BreakableObject() : Alien()
{
	
}

BreakableObject::~BreakableObject()
{
}

void BreakableObject::Start()
{
	
}

void BreakableObject::Explode()
{
	
	GameObject* new_obj = GameObject::Instantiate(object_broken, transform->GetGlobalPosition());
	new_obj->GetComponent<ExplodeChildren>()->SetVars(force, time_to_despawn);
	new_obj->transform->SetLocalScale(transform->GetLocalScale());

	auto children = new_obj->GetComponentsInChildren<ComponentRigidBody>();
	for (auto i = children.begin(); i != children.end(); ++i) {
			(*i)->SetPosition((*i)->game_object_attached->transform->GetGlobalPosition());
	}

	new_obj->GetComponent<ExplodeChildren>()->Explode();

	GameObject::Destroy(game_object);
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
{
	if(collider->game_object_attached->IsEnabled())
		if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
			collider->game_object_attached->parent->GetComponent<ComponentAudioEmitter>()->StartSound("Play_FenceDestroy");
			Explode();
		}
}
