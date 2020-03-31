#include "DialogueManager.h"
#include "EventManager.h"

DialogueManager::DialogueManager() : Alien()
{
}

DialogueManager::~DialogueManager()
{
}

void DialogueManager::Start()
{
	eventManager = (EventManager*)GameObject::FindWithName("EventManager")->GetComponentScript("EventManager");
	audioEmitter = (ComponentAudioEmitter*)game_object->GetComponent(ComponentType::A_EMITTER); 
}

void DialogueManager::Update()
{
	// Debug
	if (Input::GetKeyDown(SDL_SCANCODE_L))
	{
		Dialogue dialogue; 
		dialogue.audioData.eventName = "Hit_Sword"; 
		dialogue.priority = "Boss"; 

		InputNewDialogue(dialogue); 
	}; 
}

bool DialogueManager::InputNewDialogue(Dialogue& dialogue)
{
	if ((currentDialogue.audioData.eventName != "noName") && (eventManager->eventPriorities.at(currentDialogue.priority) > eventManager->eventPriorities.at(dialogue.priority)))
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}; 

	
	OverrideDialogue(dialogue); 

	return true;
}


void DialogueManager::OverrideDialogue(Dialogue& newDialogue)
{
	// Stop playing 
	if(audioEmitter->GetSource() != nullptr)
		audioEmitter->StopOwnSound();

	LOG("Before");
	// Set Data
	currentDialogue.audioData = newDialogue.audioData;
	currentDialogue.entityName = newDialogue.entityName;
	currentDialogue.pauseContinue = newDialogue.pauseContinue;
	currentDialogue.paused = false;
	currentDialogue.priority = newDialogue.priority;

	LOG("After");
	// Play new
	//audioEmitter->SetSwitchState(currentDialogue.audioData.groupID, currentDialogue.audioData.stateID); 
	audioEmitter->StartSound(currentDialogue.audioData.eventName); 
}