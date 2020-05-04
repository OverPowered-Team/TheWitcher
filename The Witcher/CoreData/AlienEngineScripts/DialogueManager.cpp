#include "GameManager.h"
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
	//audioEmitter = GetComponent<ComponentAudioEmitter>();
	//text = GameObject::FindWithName("SubtitlesText")->GetComponent<ComponentText>();

	//audioEmitter->ChangeVolume(0.5f); // some dialogues are low, so we can change the volume according to this (0->1)
	//LoadJSONDialogues();
}

void DialogueManager::LoadJSONDialogues()
{
	// Credits to Yessica
	std::string json_path = std::string("Configuration/Subtitles/InGameDialogues.json");
	LOG("READING %s", json_path.data());
	JSONfilepack* jsonDoc = JSONfilepack::GetJSON(json_path.c_str());
	if (jsonDoc)
	{
		JSONArraypack* dialogues = jsonDoc->GetArray("dialogues");
		if (dialogues == nullptr)
		{
			LOG("No dialogues array in fucking dialogues JSON");
			return;
		}

		do
		{
			LOG("Loading new dialogue data...");
			std::string eventName = dialogues->GetString("eventName");
			std::string subtitles = dialogues->GetString("subtitles");
			float time = dialogues->GetNumber("time");

			dialogueData.push_back(std::make_tuple(eventName, subtitles, time));

		} while (dialogues->GetAnotherNode());
	}
	else
		LOG("Couldn't open fucking dialogues JSON");


	JSONfilepack::FreeJSON(jsonDoc);
}

void DialogueManager::Update()
{
	/*if (playing)
	{
		LOG("Subtitles current: %f vs total: %f", currentDialogue.subtitlesTime.currentTime, currentDialogue.subtitlesTime.totalTime);
		if ((currentDialogue.subtitlesTime.currentTime += Time::GetDT()) >= currentDialogue.subtitlesTime.totalTime)
		{
			playing = false;
			text->SetEnable(false);
			currentDialogue.Reset();
			audioEmitter->ChangeVolume(0.5f);
		}
	}*/
	//else {
	//audioEmitter->SetState("GameVolumes", "None");
	//}

}

bool DialogueManager::InputNewDialogue(Dialogue& dialogue, float volume)
{
	// TODO: We don't have a way of knowing the sound's duration, so we won't take into account the priority
	// since we don't know when to reset the current dialogue

	/*if ((currentDialogue.audioData.eventName != "noName") && (eventManager->eventPriorities.at(currentDialogue.priority) < eventManager->eventPriorities.at(dialogue.priority)))
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}; */


	OverrideDialogue(dialogue, volume);

	playing = true;

	return true;
}

bool DialogueManager::InputNewDialogue(int index, float volume)
{
	assert((index <= (dialogueData.size() - 1)) && "Invalid dialogue index");
	if (index > (dialogueData.size() - 1))
	{
		LOG("Invalid dialogue index");
		return false;
	}

	Dialogue dialogue;
	dialogue.audioData.eventName = std::get<0>(dialogueData.at(index));
	dialogue.subtitlesText = std::get<1>(dialogueData.at(index));
	dialogue.subtitlesTime.totalTime = std::get<2>(dialogueData.at(index));

	OverrideDialogue(dialogue, volume);

	playing = true;


	return true;
}


void DialogueManager::OverrideDialogue(Dialogue& newDialogue, float volume)
{
	// Stop playing 
	audioEmitter->StopSoundByName(currentDialogue.audioData.eventName.c_str());
	LOG("Stopped playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str());

	// Set Data --> TODO: other members in "audioData"
	currentDialogue.audioData.eventName = std::string(newDialogue.audioData.eventName.c_str());
	currentDialogue.priority = std::string(newDialogue.priority.c_str());
	currentDialogue.subtitlesText = std::string(newDialogue.subtitlesText.c_str());
	currentDialogue.subtitlesTime = newDialogue.subtitlesTime;

	// Set Subtitles 
	if (text->IsEnabled() == false)
		text->SetEnable(true);
	//text->Reset(); 
	text->SetText(newDialogue.subtitlesText.c_str());


	// Play new
	//audioEmitter->SetSwitchState(newDialogue.audioData.groupID, newDialogue.audioData.stateID); 
	audioEmitter->SetState("GameVolumes", "Dialogues");
	audioEmitter->ChangeVolume(volume);
	audioEmitter->StartSound(currentDialogue.audioData.eventName.c_str());
	LOG("Started playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str());
}