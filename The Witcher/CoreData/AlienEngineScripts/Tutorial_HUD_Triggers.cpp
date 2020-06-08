#include "Tutorial_HUD_Triggers.h"
#include "Tutorial_HUD.h"
#include "PlayerController.h"

Tutorial_HUD_Triggers::Tutorial_HUD_Triggers() : Alien()
{
}

Tutorial_HUD_Triggers::~Tutorial_HUD_Triggers()
{
}

void Tutorial_HUD_Triggers::Start()
{
}

void Tutorial_HUD_Triggers::Update()
{
}

void Tutorial_HUD_Triggers::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player") == 0)
	{
		switch (trigger_type)
		{
		case TRIGGER_TYPE::ATTACK:
		{
			players_triggered_attack.push_back(col->game_object_attached->GetComponent<PlayerController>());
			game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerAttack(true);
			break;
		}
		case TRIGGER_TYPE::DASH:
		{
			players_triggered_dash.push_back(col->game_object_attached->GetComponent<PlayerController>());
			game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerDash(true);
		}
		case TRIGGER_TYPE::MAGIC:
		{
			players_triggered_magic.push_back(col->game_object_attached->GetComponent<PlayerController>());
			game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerMagic(true);
			break;
		}
		case TRIGGER_TYPE::ANY:
			break;
		}
	}
}

void Tutorial_HUD_Triggers::OnTriggerExit(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player") == 0)
	{
		switch (trigger_type)
		{
		case TRIGGER_TYPE::ATTACK:
		{
			players_triggered_attack.remove(col->game_object_attached->GetComponent<PlayerController>());
			if (players_triggered_attack.empty())
			{
				game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerAttack(false);
			}
			break;
		}
		case TRIGGER_TYPE::DASH:
		{
			players_triggered_dash.remove(col->game_object_attached->GetComponent<PlayerController>());
			if (players_triggered_dash.empty())
			{
				game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerDash(false);
			}
			break;
		}
		case TRIGGER_TYPE::MAGIC:
		{
			players_triggered_magic.remove(col->game_object_attached->GetComponent<PlayerController>());
			if (players_triggered_magic.empty())
			{
				game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerMagic(false);
			}
			break;
		}
		case TRIGGER_TYPE::ANY:
			break;
		}
	}
}
