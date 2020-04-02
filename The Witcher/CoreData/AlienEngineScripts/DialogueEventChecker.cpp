#include "DialogueEventChecker.h"
#include "DialogueManager.h"
#include "PlayerController.h"
#include "EventManager.h"

DialogueEventChecker::DialogueEventChecker() : Alien()
{
}

DialogueEventChecker::~DialogueEventChecker()
{
}

void DialogueEventChecker::Start()
{
	dialogueManager = (DialogueManager*)GetComponentScript("DialogueManager");
	eventManager = (EventManager*)GetComponentScript("EventManager");
     
	// TODO: this right now refers to one player
	Dialogue dialogue; 
	dialogue.audioData.eventName = std::get<0>(logicalDialogueData.at(0));
	dialogue.subtitlesText = std::get<1>(logicalDialogueData.at(0));
	dialogue.subtitlesTime.totalTime = std::get<2>(logicalDialogueData.at(0));

    // TODO: push to logicalDialogueData 
}

void DialogueEventChecker::LoadJSONLogicalDialogues() // TODO
{
	// Credits to Yessica
	std::string json_path = std::string("Configuration/Subtitles/InGameLogicalDialogues.json");
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

			logicalDialogueData.push_back(std::make_tuple(eventName, subtitles, time));

		} while (dialogues->GetAnotherNode());
	}
	else
		LOG("Couldn't open fucking dialogues JSON");


	JSONfilepack::FreeJSON(jsonDoc);
}

void DialogueEventChecker::Update()
{
	for (int i = 0; i < checkers.size(); ++i)
	{
		auto value = checkers.at(i);
		// TODO: check priority with current one and don't call function
		Dialogue dialogue = std::get<1>(value); 
		std::get<0>(value)(dialogue);
	}


}

// Chekers
void DialogueEventChecker::CheckKills(Dialogue& dialogue)
{
	static float checkTime = 3.0f, currentTimeGeralt = 0.0f, currentTimeYennefer = 0.0f;
	static uint checkKills = 5;
	static uint geraltStreak = 0, YenneferStreak = 0;
	static uint geraltLastKills = 0, YenneferLastKills = 0;
	static PlayerController* Geralt = (PlayerController*)GameObject::FindWithName("Geralt")->GetComponentScript("PlayerController");
	static PlayerController* Yennefer = (PlayerController*)GameObject::FindWithName("Yennefer")->GetComponentScript("PlayerController");
	
	auto Calculate = [&](PlayerController* player, uint& lastKills, uint totalKills, uint& streak, float& currentTime) {
	
		if (lastKills != player->player_data.total_kills)
			streak += (player->player_data.total_kills - lastKills);

		if ((currentTime += Time::GetDT()) >= checkTime)
		{
			if (streak >= checkKills)
				eventManager->ReceiveDialogueEvent(dialogue);
	
			currentTime = 0.f;
			streak = 0;
		}

		lastKills = totalKills;
	};


	Calculate(Geralt, geraltLastKills, Geralt->player_data.total_kills, geraltStreak, currentTimeGeralt); 
	Calculate(Yennefer, YenneferLastKills, Yennefer->player_data.total_kills, YenneferStreak, currentTimeYennefer);

}