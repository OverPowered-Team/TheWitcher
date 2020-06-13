#include "BreakDoor.h"
#include "FenceDoor.h"
#include "AttackTrigger.h"
#include "PlayerController.h"

BreakDoor::BreakDoor() : Alien()
{
}

BreakDoor::~BreakDoor()
{
}

void BreakDoor::OnTriggerEnter(ComponentCollider* collider)
{
	if (collider->game_object_attached->IsEnabled())
	{
		if (strcmp("PlayerAttack", collider->game_object_attached->GetTag()) == 0) {
			++current_hits;
			if (current_hits >= hits_to_broke)
			{
				game_object->parent->GetComponent<FenceDoor>()->Explode();

				GameObject* player = collider->game_object_attached->GetComponent<AttackTrigger>()->player_obj;

				Input::DoRumble(player->GetComponent<PlayerController>()->controller_index, 0.05f, 1.f);

				auto audio = player->GetComponent<ComponentAudioEmitter>();
				//auto audio = collider->game_object_attached->parent->parent->parent->parent->parent->parent->parent->parent->parent->parent->parent->GetComponent<ComponentAudioEmitter>();
				if (audio)
					audio->StartSound("Play_FenceDestroy");
			}
			else {
				Input::DoRumble(collider->game_object_attached->GetComponent<AttackTrigger>()->player_obj->GetComponent<PlayerController>()->controller_index, 0.5f, 1.f);
			}
		}
	}
}