#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "RootLeshen.h"

RootLeshen::RootLeshen() : Alien()
{
}

RootLeshen::~RootLeshen()
{
}

void RootLeshen::Start()
{
	float closer_distance, player_1_distance, player_2_distance = 0.0f;
	player_1_distance = transform->GetGlobalPosition().Distance(GameManager::manager->player_manager->players[0]->game_object->transform->GetGlobalPosition());
	player_2_distance = transform->GetGlobalPosition().Distance(GameManager::manager->player_manager->players[1]->game_object->transform->GetGlobalPosition());

	(player_1_distance < player_2_distance) ? target = 1 : target = 2;
}

void RootLeshen::Update()
{
	direction = (GameManager::manager->player_manager->players[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
	transform->AddPosition(direction * speed);

	if(life_time <= total_life_time)
		life_time += Time::GetDT();
	else
		GameObject::Destroy(game_object);
}

void RootLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl) {
			LOG("Rooted");
		}
		else {
			LOG("There's no Player Controller in GO in ArrowScript!");
		}

		GameObject::Destroy(game_object);
	}
	else if (strcmp(collider->game_object_attached->GetTag(), "Enemy") != 0)
	{
		GameObject::Destroy(game_object);
	}
}