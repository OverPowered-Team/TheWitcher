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
	/*if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {

		PlayerController* player = collider->game_object_attached->GetComponent<PlayerController>();
		if (player && !player)
		{
			float3 knock = direction * knock_back;
			float damage_dealt = enemy->GetDamaged(damage, player, knock);

			player->player_data.total_damage_dealt += damage_dealt;

			if (!enemy->IsDead())
				GameObject::Destroy(this->game_object);
		}
	}
	else if (strcmp(collider->game_object_attached->GetTag(), "Player") != 0)
	{
		GameObject::Destroy(this->game_object);
	}*/
}