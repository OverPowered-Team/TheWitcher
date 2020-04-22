#include "RockDownHill.h"

RockDownHill::RockDownHill() : Alien()
{
}

RockDownHill::~RockDownHill()
{
}

void RockDownHill::Start()
{
	rb = GetComponent<ComponentRigidBody>();
}

void RockDownHill::Update()
{
	rb->SetPosition(transform->GetGlobalPosition() + direction * speed * Time::GetDT());
}

void RockDownHill::CalculateDirection()
{
	direction = (end_pos - transform->GetGlobalPosition()).Normalized();
}

void RockDownHill::OnTriggerEnter(ComponentCollider* trigger)
{
	LOG("ENTERED IN COLLIDER");
	if (strcmp(trigger->game_object_attached->GetTag(), "RockEnd") == 0) {
		LOG("ENTERED IN ROCKEND");
		GameObject::Destroy(game_object);
	}
}
