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
	LOG("Dialogue trigger activated, dialogue audio event name: %s", dialogue.audioData.eventName.c_str()); 
	eventManager->ReceiveDialogueEvent(this->dialogue);

	// TODO: we can't delete this, won't play sound? The object is unrelated with the emitter (XD)
	if (once)
		game_object->ToDelete();
}

