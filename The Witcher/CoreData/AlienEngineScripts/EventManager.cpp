#include "EventManager.h"
#include "PlayerController.h"

EventManager::EventManager() : Alien()
{
}

EventManager::~EventManager()
{
}

void EventManager::Start()
{
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
