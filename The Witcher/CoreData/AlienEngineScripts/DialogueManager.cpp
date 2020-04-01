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
	audioEmitter->StopSoundByName(currentDialogue.audioData.eventName.c_str()); 
	LOG("Stopped playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str()); 

	// Set Data --> TODO: other members in "audioData"
	currentDialogue.audioData.eventName = std::string(newDialogue.audioData.eventName.c_str()); 
	currentDialogue.priority = std::string(newDialogue.priority.c_str());
	currentDialogue.subtitlesText = std::string(newDialogue.subtitlesText.c_str());

	/*
	// Set Subtitles --> TODO: crashes
	if(text->IsEnabled() == false)
		text->SetEnable(true);
	text->Reset(); 
	text->text = std::string(newDialogue.subtitlesText); 
	*/

	// Play new
	//audioEmitter->SetSwitchState(newDialogue.audioData.groupID, newDialogue.audioData.stateID); 
	audioEmitter->StartSound(currentDialogue.audioData.eventName.c_str());
	LOG("Started playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str());
}
