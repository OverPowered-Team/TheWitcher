#include "ArrowScript.h"
#include "PlayerController.h"

ArrowScript::ArrowScript() : Alien()
{
}

ArrowScript::~ArrowScript()
{
}

void ArrowScript::Start()
{
	current_time = Time::GetGameTime();
}

void ArrowScript::Update()
{
	if ((Time::GetGameTime() - current_time) > life_time)
		GameObject::Destroy(game_object);
}

void ArrowScript::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune)
		{
			GameObject::Destroy(game_object);
			if (!player_ctrl->godmode)
			{
				player_ctrl->ReceiveDamage(damage);
			}
		}
		else
			LOG("There's no Player Controller in GO in ArrowScript!");
	}
	else if(strcmp(collider->game_object_attached->GetTag(), "Enemy") != 0)
	{
		GameObject::Destroy(game_object);
	}
}
