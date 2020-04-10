#include "PlayerController.h"
#include "PlayerManager.h"

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
}

void PlayerManager::Update()
{
	if (ultimate_buttons_pressed == players.size() && collective_ultimate_charge == max_ultimate_charge)
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

void PlayerManager::IncreaseUltimateCharge(uint value)
{
	if (ultimate_is_active)
		return;

	collective_ultimate_charge += value;
	//ui here?

	if (collective_ultimate_charge > max_ultimate_charge)
		collective_ultimate_charge = max_ultimate_charge;
}

void PlayerManager::ActivateUltimate()
{
	Invoke(std::bind(&PlayerManager::CancelUltimate, this), ultimate_time);
	collective_ultimate_charge = 0;
	ultimate_is_active = true;

	Time::SetScaleTime(ultimate_effect_value);
	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->OnUltimateActivation(1/ultimate_effect_value);
	}
}

void PlayerManager::CancelUltimate()
{
	Time::SetScaleTime(1.0f);
	ultimate_is_active = false;

	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->OnUltimateDeactivation(1/ultimate_effect_value);
	}
}
