#include "Head_Script.h"
#include "Scale_Win.h"

Head_Script::Head_Script() : Alien()
{
	script_scale = (Scale_Win*)GameObject::FindWithName("Scale")->GetComponent<Scale_Win>();
}

Head_Script::~Head_Script()
{
}
void Head_Script::OnTriggerEnter(ComponentCollider* collider)
{
	if (first)
	{
		LOG("Trigger");
		if (collider->game_object_attached == GameObject::FindWithName("Left_Scale"))
		{
			script_scale->player1_points += points;
		}
		else
		{
			script_scale->player2_points += points;
		}

		script_scale->CalculateInclination();
		first = false;
	}
}
