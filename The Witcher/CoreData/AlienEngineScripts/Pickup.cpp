#include "PlayerController.h"
#include "Pickup.h"

void Pickup::Start()
{
	start_time = Time::GetGameTime();
}

void Pickup::Update()
{
	if ((Time::GetGameTime() - start_time) > duration)
		GameObject::Destroy(this->game_object);
}

void Pickup::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {

		PlayerController* player = collider->game_object_attached->GetComponent<PlayerController>();
		if (player && player->state->type != StateType::DEAD)
		{
			player->IncreaseStat(stat_to_change, value);
			GameObject::Destroy(this->game_object);
		}
	}
}