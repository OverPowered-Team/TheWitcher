#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Leshen.h"
#include "RootLeshen.h"

RootLeshen::RootLeshen() : Alien()
{
}

RootLeshen::~RootLeshen()
{
}

void RootLeshen::Start()
{
	root_time = 0.0f;
	life_time = 0.0f;

	state = ROOTSTATE::SEEK;
}

void RootLeshen::Update()
{
	if (state == ROOTSTATE::SEEK) {
		direction = (GameManager::instance->player_manager->players[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		direction.y = 0;
		transform->AddPosition(direction * speed);

		if (life_time <= total_life_time)
			life_time += Time::GetDT();
		else {
			GameObject::Destroy(game_object);
			leshen->current_action->state = Leshen::ActionState::ENDED;
		}
	}
	else if (state == ROOTSTATE::ROOT) {
		transform->AddPosition(rooted_effect_direction * root_speed);
		if (root_time <= total_root_time)
			root_time += Time::GetDT();
		else {
			GameObject::Destroy(game_object);
		}
	}
}

void RootLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune && !player_ctrl->is_rooted) {
			state = ROOTSTATE::ROOT;
			transform->SetGlobalPosition(collider->game_object_attached->transform->GetGlobalPosition());
			player_ctrl->ApplyRoot(total_root_time);
			leshen->EndRootAction(game_object);
		}
		else {
			LOG("There's no Player Controller in GO in ArrowScript!");
		}
	}
	else if (strcmp(collider->game_object_attached->GetTag(), "Enemy") != 0)
	{
		GameObject::Destroy(game_object);
	}
}