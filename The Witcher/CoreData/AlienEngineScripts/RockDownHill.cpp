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
	rb->SetRotation(transform->GetGlobalRotation() * Quat::RotateAxisAngle(axis_rot, rot_speed * Time::GetDT()));
}

void RockDownHill::CalculateDirection(const float3& end_pos)
{
	direction = (end_pos - transform->GetGlobalPosition()).Normalized();
	axis_rot = Cross(float3::unitY(), direction);
}

void RockDownHill::SetMoveAndRotationSpeed(const float& rot_speed, const float& speed)
{
	this->rot_speed = rot_speed;
	this->speed = speed;
}

void RockDownHill::OnTriggerEnter(ComponentCollider* trigger)
{
	if (strcmp(trigger->game_object_attached->GetTag(), "RockEnd") == 0) {
		GameObject::Destroy(game_object);
	}
}
