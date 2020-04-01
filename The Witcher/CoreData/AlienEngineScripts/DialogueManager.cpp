#include "DialogueManager.h"
#include "EventManager.h"
#include "..\..\ComponentText.h"

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
	text = (ComponentText*)GameObject::FindWithName("SubtitlesText")->GetComponent(ComponentType::UI_TEXT);
 
}

void DialogueManager::Update()
{
	if ((subtitlesTime.currentTime += Time::GetDT()) >= subtitlesTime.totalTime)
	{
		subtitlesTime.currentTime = 0.0f; 
		text->SetEnable(false); 
	}

	// Debug
	if (Input::GetKeyDown(SDL_SCANCODE_1))
	{
		Dialogue dialogue; 
		dialogue.audioData.eventName = "Hit_Sword"; 
		dialogue.subtitlesText = "Subtitles subtitles a a a subtitles e e e"; 
		dialogue.priority = "Boss"; 

		InputNewDialogue(dialogue); 
	}; 


	if (Input::GetKeyDown(SDL_SCANCODE_2))
	{
		Dialogue dialogue;
		dialogue.audioData.eventName = "PlayGhouls";
		dialogue.subtitlesText = "Subtitlessssssssss 2 ANOTHER TEXT subtitles";
		dialogue.priority = "Enemies";

		InputNewDialogue(dialogue);
	};


}

bool DialogueManager::InputNewDialogue(Dialogue& dialogue)
{
	// TODO: We don't have a way of knowing the sound's duration, so we won't take into account the priority
	// since we don't know when to reset the current dialogue
	
	/*if ((currentDialogue.audioData.eventName != "noName") && (eventManager->eventPriorities.at(currentDialogue.priority) < eventManager->eventPriorities.at(dialogue.priority)))
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}; */

	
	OverrideDialogue(dialogue); 

	return true;
}


void DialogueManager::OverrideDialogue(Dialogue& newDialogue)
{
	// Stop playing 
	audioEmitter->StopSoundByName(currentDialogue.audioData.eventName); 

	// Set Data
	currentDialogue.audioData = newDialogue.audioData;
	currentDialogue.entityName = newDialogue.entityName;
	currentDialogue.pauseContinue = newDialogue.pauseContinue;
	currentDialogue.paused = false;
	currentDialogue.priority = newDialogue.priority;

	LOG("About to change subtitles...");

	// Set Subtitles --> // TODO: CRASH
	/*if(text->IsEnabled() == false)
		text->SetEnable(true);
	text->text = std::string(newDialogue.subtitlesText); */

	LOG("After changing subtitles...");

	// Play new
	//audioEmitter->SetSwitchState(currentDialogue.audioData.groupID, currentDialogue.audioData.stateID); 
	audioEmitter->StartSound(currentDialogue.audioData.eventName); 
}
