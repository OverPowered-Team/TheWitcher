#include "GameManager.h"
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
	life_time = 0.0f;
	setted_direction = false;
}

void CrowsLeshen::Update()
{
	if (!setted_direction) {
		direction = (leshen->player_controllers[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg()) * Maths::Deg2Rad());
		transform->SetGlobalRotation(rot);
		setted_direction = true;
	}
	transform->AddPosition(direction * speed);

	if (life_time <= total_life_time)
		life_time += Time::GetDT();
	else {
		Destroy(game_object);
	}
}

void CrowsLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune) {
			player_ctrl->ReceiveDamage(10.0f);
			Destroy(game_object);
		}
	}
}
