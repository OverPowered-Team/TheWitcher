#include "GameManager.h"
#include "DialogueTriggerSimple.h"
#include "EventManager.h"
#include "PlayerController.h"

DialogueTriggerSimple::DialogueTriggerSimple() : Alien()
{
}

DialogueTriggerSimple::~DialogueTriggerSimple()
{
}

void DialogueTriggerSimple::OnTriggerEnter(ComponentCollider* collider)
{
	if ((strcmp(collider->game_object_attached->GetTag(), "Player") != 0))
		return;

	PlayerController* pl = collider->game_object_attached->GetComponent<PlayerController>();
	bool isGeralt = pl->player_data.type == PlayerController::PlayerType::GERALT; 

	if (!specificEntity || (specificEntity && (isGeralt == this->Geralt)))
	{
		GameManager::instance->event_manager->ReceiveDialogueEvent(diaLogueIndex, volume);
		if (once)
			game_object->ToDelete();
	}
}

