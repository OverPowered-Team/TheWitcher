#include "RockDownHill.h"
#include "PlayerController.h"

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
	else if (strcmp(trigger->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = trigger->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && player_ctrl->state != PlayerController::PlayerState::DEAD)
		{
			Destroy(game_object);
			player_ctrl->ReceiveDamage(damage, direction.Normalized() * 0.3f);
		}
	}
}
