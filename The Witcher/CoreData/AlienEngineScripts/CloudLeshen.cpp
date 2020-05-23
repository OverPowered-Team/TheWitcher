#include "CloudLeshen.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "PlayerManager.h"

CloudLeshen::CloudLeshen() : Alien()
{
}

CloudLeshen::~CloudLeshen()
{
}

void CloudLeshen::Start()
{
}

void CloudLeshen::Update()
{
	if (cloud_damage_tick_timer <= cloud_damage_tick) {
		cloud_damage_tick_timer += Time::GetDT();
		can_damage = false;
	}
	else {
		cloud_damage_tick_timer = 0.0f;
		if (player_entered[0]) {
			GameManager::instance->player_manager->players[0]->ReceiveDamage(cloud_damage);
		}

		if (player_entered[1]) {
			GameManager::instance->player_manager->players[1]->ReceiveDamage(cloud_damage);
		}
	}
}

void CloudLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		LOG("PlayerEntered");
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player && can_damage)
		{
			if(player->controller_index == 0)
				player_entered[0] = true;
			else
				player_entered[1] = true;
		}
	}
}

void CloudLeshen::OnTriggerExit(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		LOG("PlayerExit");
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player && can_damage)
		{
			if (player->controller_index == 0)
				player_entered[0] = false;
			else
				player_entered[1] = false;
		}
	}
}
