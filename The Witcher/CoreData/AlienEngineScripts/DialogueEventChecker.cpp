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

	// FUNCTIONS MUST BE IN THE SAME ORDER AS DIALOGUES IN THE JSON !!!
	LoadJSONLogicalDialogues(); 
	checkers.push_back(&CheckKills);
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

			Dialogue d; 
			d.audioData.eventName = eventName; 
			d.subtitlesText = subtitles; 
			d.subtitlesTime.totalTime = time; 

			logicalDialogueData.push_back(d); 

		} while (dialogues->GetAnotherNode());
	}
	else
		LOG("Couldn't open fucking dialogues JSON");


	JSONfilepack::FreeJSON(jsonDoc);
}

void DialogueEventChecker::Update()
{
	// TODO: check priority with current one and don't call function
	for (int i = 0; i < checkers.size(); ++i)
	{
		auto func = checkers.at(i);
		Dialogue dialogue = logicalDialogueData.at(i); 
	
		if(func(dialogue))
			eventManager->ReceiveDialogueEvent(dialogue);
	}


}

// Chekers
bool CheckKills(Dialogue& dialogue)
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
				return true; 
	
			currentTime = 0.f;
			streak = 0;
		}

		lastKills = totalKills;
	};

	// TODO: Yennefer, which is the attack button on keyboard? XD
	Calculate(Geralt, geraltLastKills, Geralt->player_data.total_kills, geraltStreak, currentTimeGeralt); 
	//Calculate(Yennefer, YenneferLastKills, Yennefer->player_data.total_kills, YenneferStreak, currentTimeYennefer);

	return false; 
}