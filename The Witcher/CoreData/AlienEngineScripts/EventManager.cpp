#include "EventManager.h"
#include "PlayerController.h"

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

void EventManager::ReceiveDialogueEvent(const char* audioName, unsigned int priority, bool pauseContinue, const char* entityName, float delay) const
{
	// TODO: send this to the dialogue script
	/*if (delay == 0.f)

	else
		Invoke();*/ 
}

