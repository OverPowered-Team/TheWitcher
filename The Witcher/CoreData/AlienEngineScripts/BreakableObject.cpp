#include "BreakableObject.h"

BreakableObject::BreakableObject() : Alien()
{
}

BreakableObject::~BreakableObject()
{
}

void BreakableObject::Start()
{
}

void BreakableObject::Update()
{
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
		GameObject* new_obj = GameObject::Instantiate(object_broken, transform->GetGlobalPosition(), game_object->parent);
		new_obj->transform->SetGlobalRotation(transform->GetGlobalRotation());
		//((ComponentRigidBody*)new_obj->GetComponent(ComponentType::RIGID_BODY))->AddTorque(-float3::unitZ());

		//std::vector<GameObject*> children = new_obj->GetChildren();
		/*for (auto i = children.begin(); i != children.end(); ++i) {
			(*i).
		}*/

		GameObject::Destroy(game_object);
	}
}
