#include "RockThrow.h"
#include "PlayerController.h"
#include "Enemy.h"

RockThrow::RockThrow() : Alien()
{
}

RockThrow::~RockThrow()
{
}

void RockThrow::Start()
{
}

void RockThrow::Update()
{
	if (timer < lifetime)
		timer += Time::GetDT();
	else {
		Destroy(game_object);
	}
}

void RockThrow::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune) {
			float3 dir = (collider->game_object_attached->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
			player_ctrl->ReceiveDamage(10.0f, dir*10);
		}
	}
	if (strcmp(collider->game_object_attached->GetTag(), "Enemy") == 0) {
		Enemy* enemy = collider->game_object_attached->GetComponent<Enemy>();
		if (enemy) {
			float3 dir = (collider->game_object_attached->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
			enemy->GetDamaged(10, dir * 10);
		}
	}
	Destroy(game_object);
}
