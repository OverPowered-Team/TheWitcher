#include "DialogueManager.h"
#include "PlayerController.h"
#include "EventManager.h"

DialogueManager::DialogueManager() : Alien()
{
}

DialogueManager::~DialogueManager()
{
	if (currentDialogue)
		RELEASE(currentDialogue); 
	if (pausedDialogue)
		RELEASE(pausedDialogue); 
}

void DialogueManager::Start()
{

	eventManager = (EventManager*)GameObject::FindWithName("EventManager")->GetComponentScript("EventManager");
}

void DialogueManager::Update()
{
}

bool DialogueManager::InputNewDialogue(Dialogue& dialogue) const
{
	/*if (currentDialogue && eventManager->GetEventPriorities().at(currentDialogue->priority) > eventManager->GetEventPriorities().at(dialogue.priority))
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}*/

		
	return true;
	

}
