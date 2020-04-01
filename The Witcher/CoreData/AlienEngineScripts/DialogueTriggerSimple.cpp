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

void DialogueTriggerSimple::Update()
{
	static bool lastHitToPlay = hitToPlay; 

	if (lastHitToPlay != hitToPlay)
		EmitDialogue(); 
	lastHitToPlay = hitToPlay; 

}
void DialogueTriggerSimple::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
		EmitDialogue(); 
}

void  DialogueTriggerSimple::EmitDialogue()
{
	LOG("Dialogue trigger activated"); 
	eventManager->ReceiveDialogueEvent(this->dialogue);

	if (once)
		game_object->ToDelete();
}

