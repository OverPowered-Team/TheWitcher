#include "EventManager.h"
#include "PlayerController.h"
#include "DialogueManager.h"

EventManager::EventManager() : Alien()
{
}

EventManager::~EventManager()
{
	eventPriorities.clear(); 
}

void EventManager::Start()
{
	eventPriorities =
	{
		{"Boss", 0},
	    {"Narrative", 0},
		{"Enemies", 7}
	}; 
	

	players_size = GameObject::FindGameObjectsWithTag("Player", &players_go);
	for (int i = 0; i < players_size; ++i) {
		players.push_back((PlayerController*)players_go[i]->GetComponentScript("PlayerController"));
	}

	//dialogueManager = (DialogueManager*)GameObject::FindWithName("DialogueManager")->GetComponentScript("DialogueManager");

}

void EventManager::Update()
{
}

void EventManager::OnPlayerDead(PlayerController* player_dead)
{
	for (int i = 0; i < players.size(); ++i) {
		players[i]->OnPlayerDead(player_dead);
	}
}

void EventManager::OnPlayerRevive(PlayerController* player_revived)
{
	for (int i = 0; i < players.size(); ++i) {
		players[i]->OnPlayerRevived(player_revived);
	}
}

void EventManager::ReceiveDialogueEvent(Dialogue &dialogue, float delay) const
{
	bool c = (eventPriorities.find(dialogue.priority) == eventPriorities.end());
	assert(!c && "Priority not valid");

	if (!c)
	{
		LOG("Priority not valid");
		return;
	}

	//eventPriorities.at(dialogue.priority)

	// TODO: send this to the dialogue script
	dialogueManager->InputNewDialogue(dialogue);

}

