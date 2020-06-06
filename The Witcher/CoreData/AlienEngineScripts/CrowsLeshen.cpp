#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Leshen.h"
#include "CrowsLeshen.h"

CrowsLeshen::CrowsLeshen() : Alien()
{
}

CrowsLeshen::~CrowsLeshen()
{
}

void CrowsLeshen::Start()
{
	speed = 0.2f;
	life_time = 0.0f;
	total_life_time = 3.0f;
	direction = { 1, 0, 1 };
	target = 0;
	setted_direction = false;
	leshen = nullptr;

	max_track_distance = 4.0f;
	tracking = true;
}

void CrowsLeshen::Update()
{
	if (!setted_direction) {
		direction = (leshen->player_controllers[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		transform->SetGlobalRotation(rot);
		setted_direction = true;
	}

	float distance_to_player = transform->GetGlobalPosition().Distance(leshen->player_controllers[target]->transform->GetGlobalPosition());

	if (tracking) {
		if (distance_to_player > max_track_distance) {
			direction = -(leshen->player_controllers[target]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
			float desired_angle = atan2f(direction.z, direction.x);
			desired_angle = -(desired_angle * Maths::Rad2Deg() + 90.f) * Maths::Deg2Rad();
			Quat rot = Quat::RotateAxisAngle(float3::unitY(), desired_angle);
			Quat current_rot = Quat::Slerp(transform->GetGlobalRotation(), rot, 0.05f);
			transform->SetGlobalRotation(current_rot);
		}
		else {
			tracking = false;
		}
	}

	transform->AddPosition(transform->forward * speed);

	if (life_time <= total_life_time) {
		life_time += Time::GetDT();
	}
	else {
		GameManager::instance->particle_pool->ReleaseInstance("Crow", game_object);
		Reset();
	}
}

void CrowsLeshen::Reset()
{
	speed = 0.2f;
	life_time = 0.0f;
	total_life_time = 3.0f;
	direction = { 1, 0, 1 };
	target = 0;
	setted_direction = false;
	leshen = nullptr;

	max_track_distance = 4.0f;
	tracking = true;
}

void CrowsLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune) {
			player_ctrl->ReceiveDamage(10.0f);
			GameManager::instance->particle_pool->ReleaseInstance("Crow", game_object);
			Reset();
		}
	}
}
