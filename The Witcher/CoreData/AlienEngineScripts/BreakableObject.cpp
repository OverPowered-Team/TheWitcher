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
	std::vector<ComponentParticleSystem*> particle_gos = game_object->GetChild("Particles")->GetComponentsInChildren<ComponentParticleSystem>();

	for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
		particles.insert(std::pair((*it)->game_object_attached->GetName(), (*it)));
		(*it)->OnStop();
	}
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

	particles["explosion"]->Restart();
	particles["smoke"]->Restart();

	GameObject::Destroy(game_object);
}

void BreakableObject::OnTriggerEnter(ComponentCollider* collider)
{
	if(collider->game_object_attached->IsEnabled())
		if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
			Explode();
		}
}
