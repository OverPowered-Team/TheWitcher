#include "DialogueManager.h"
#include "PlayerController.h"

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
}

void DialogueManager::Update()
{
}

bool DialogueManager::InputNewDialogue(const char* audioName, unsigned int priority, bool pauseContinue, const char* entityName) const
{
	if (currentDialogue && currentDialogue->priority > priority)
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}
		

	

}
