#include "Head_Script.h"
#include "Scale_Win.h"

Head_Script::Head_Script() : Alien()
{
}

Head_Script::~Head_Script()
{
}

void Head_Script::Start()
{
	script_scale = GameObject::FindWithName("Scale")->GetComponent<Scale_Win>();
}

void Head_Script::OnTriggerEnter(ComponentCollider* collider)
{
	if (first)
	{
		game_object->GetComponent<ComponentAudioEmitter>()->StartSound();
		if (collider->game_object_attached == GameObject::FindWithName("Right_Scale"))
		{
			script_scale->current_points2 = script_scale->player2_points;
			script_scale->player2_points += points;
		}
		else
		{
			script_scale->current_points1 = script_scale->player1_points;
			script_scale->player1_points += points;
		}

		script_scale->CalculateInclination();
		first = false;
	}
}
