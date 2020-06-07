#include "CiriNado.h"
#include "GameManager.h"
#include "PlayerController.h"

CiriNado::CiriNado() : Alien()
{
}

CiriNado::~CiriNado()
{
}

void CiriNado::Start()
{
}

void CiriNado::Update()
{
}

void CiriNado::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune) {
			knock_back = (player_ctrl->game_object->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
			player_ctrl->ReceiveDamage(0, knock_back * knock_force);
		}
	}
}
