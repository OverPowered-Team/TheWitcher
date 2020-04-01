#include "DialogueTriggerSimple.h"
#include "EventManager.h"
#include "DialogueManager.h"

DialogueTriggerSimple::DialogueTriggerSimple() : Alien()
{
}

DialogueTriggerSimple::~DialogueTriggerSimple()
{
}

void DialogueTriggerSimple::Start()
{
	eventManager = (EventManager*)GameObject::FindWithName("EventManager")->GetComponentScript("EventManager");

}

void DialogueTriggerSimple::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		Dialogue dialogue;
		dialogue.audioData.eventName = "Hit_Sword";
		dialogue.priority = "Boss";

		eventManager->ReceiveDialogueEvent(dialogue);
	}; 
}

