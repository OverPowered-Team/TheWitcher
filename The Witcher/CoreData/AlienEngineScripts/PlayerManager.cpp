#include "PlayerController.h"
#include "PlayerManager.h"

#define MAX_ULTIMATE_CHARGE 200

PlayerManager::PlayerManager() : Alien()
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::Start()
{
	GameObject** players_go;
	uint players_size = GameObject::FindGameObjectsWithTag("Player", &players_go);

	for (int i = 0; i < players_size; ++i) {
		players.push_back((PlayerController*)players_go[i]->GetComponentScript("PlayerController"));
	}

	GameObject::FreeArrayMemory((void***)&players_go);

	//testing
	collective_ultimate_charge = MAX_ULTIMATE_CHARGE;
}

void PlayerManager::Update()
{
	if (ultimate_buttons_pressed == players.size() && collective_ultimate_charge == MAX_ULTIMATE_CHARGE)
	{
		ActivateUltimate();
	}
}

void PlayerManager::OnPlayerDead(PlayerController* dead_player)
{
	players_dead.push_back(dead_player);

	if (players_dead.size() == players.size())
	{
		// Put this on a UI Manager so we can do Game_Manager->ui_manager->in_game->YouDied();
		//((InGame_UI*)GameObject::FindWithName("UI_InGame")->GetComponentScript("InGame_UI"))->YouDied(); 
	}
}

void PlayerManager::OnPlayerRevive(PlayerController* revived_player)
{
	for (std::vector<PlayerController*>::iterator it = players_dead.begin(); it != players_dead.end(); ++it) {
		if ((*it) == revived_player) {
			players_dead.erase(it);
			break;
		}
	}
}

void PlayerManager::ActivateUltimate()
{
	LOG("ULTIMATE");
	Invoke(std::bind(&PlayerManager::CancelUltimate, this), 5);
	collective_ultimate_charge = 0;

	Time::SetScaleTime(0.5f);
	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->OnUltimateActivation();
	}
}

void PlayerManager::CancelUltimate()
{
	LOG("ULTIMATE CANCELED");
}
