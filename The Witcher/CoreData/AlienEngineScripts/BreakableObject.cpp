#include "BreakableObject.h"
#include "ExplodeChildren.h"

BreakableObject::BreakableObject() : Alien()
{
}

BreakableObject::~BreakableObject()
{
}

void BreakableObject::Explode()
{
	GameObject* new_obj = GameObject::Instantiate(object_broken, transform->GetGlobalPosition());
	new_obj->GetComponent<ExplodeChildren>()->SetVars(force, time_to_despawn);
	new_obj->transform->SetLocalScale(transform->GetLocalScale());

	std::vector<GameObject*> children = new_obj->GetChildren();
	for (auto i = children.begin(); i != children.end(); ++i) {
		ComponentRigidBody* rb = (*i)->GetComponent<ComponentRigidBody>();
		rb->SetPosition((*i)->transform->GetGlobalPosition());
	}

	new_obj->GetComponent<ExplodeChildren>()->Explode();


	GameObject::Destroy(game_object);
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
{
	if(collider->game_object_attached->IsEnabled())
		if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
			Explode();
		}
}
