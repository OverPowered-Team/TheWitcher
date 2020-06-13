#include "Wagonnette_Triggers.h"
#include "Wagonnete_UI.h"
Wagonnette_Triggers::Wagonnette_Triggers() : Alien()
{
}

Wagonnette_Triggers::~Wagonnette_Triggers()
{
}

void Wagonnette_Triggers::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player") == 0)
	{
		switch (type)
		{
		case TRIGGER_TYPE::DUCKING:
		{
			GameObject::FindWithName("Wagonnette_UI")->GetComponent<Wagonnete_UI>()->ActivateTrigger(TRIGGER_TYPE::DUCKING);
			break;
		}
		case TRIGGER_TYPE::DODGING:
		{
			GameObject::FindWithName("Wagonnette_UI")->GetComponent<Wagonnete_UI>()->ActivateTrigger(TRIGGER_TYPE::DODGING);
			break;
		}
		}
	}
}
