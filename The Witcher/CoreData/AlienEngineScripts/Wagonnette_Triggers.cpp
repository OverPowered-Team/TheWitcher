#include "Wagonnette_Triggers.h"

Wagonnette_Triggers::Wagonnette_Triggers() : Alien()
{
}

Wagonnette_Triggers::~Wagonnette_Triggers()
{
}

void Wagonnette_Triggers::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player"))
	{
		switch (type)
		{
		case TRIGGER_TYPE::DUCKING:
		{
			break;
		}
		case TRIGGER_TYPE::DODGING:
		{
			break;
		}
		}
	}
}
