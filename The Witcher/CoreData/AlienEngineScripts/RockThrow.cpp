#include "RockThrow.h"
#include "PlayerController.h"

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

void RockThrow::OnCollisionEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune) {
			player_ctrl->ReceiveDamage(10.0f);
		}
	}
	Destroy(game_object);
}
