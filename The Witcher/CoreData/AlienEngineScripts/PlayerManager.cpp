#include "PlayerController.h"
#include "PlayerManager.h"
#include "UltiBar.h"
#include "InGame_UI.h"
#include "Scores_Data.h"

PlayerManager::PlayerManager() : Alien()
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::Start()
{
	auto player_go = GameObject::FindGameObjectsWithTag("Player");
	for (auto i = player_go.begin(); i != player_go.end(); ++i) {
		players.push_back((*i)->GetComponent<PlayerController>());
	}

	if(GameObject::FindWithName("Ulti_Bar"))
		ulti_bar = GameObject::FindWithName("Ulti_Bar")->GetComponent<UltiBar>();
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
		Scores_Data::player1_kills = players[0]->player_data.total_kills;
		Scores_Data::player2_kills = players[1]->player_data.total_kills;

		Scores_Data::dead = true;
		if(GameObject::FindWithName("UI_InGame"))
			GameObject::FindWithName("UI_InGame")->GetComponent<InGame_UI>()->YouDied(); 
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

	if (collective_ultimate_charge >= max_ultimate_charge)
	{
		collective_ultimate_charge = max_ultimate_charge;
		// UI
		if(ulti_bar)
			ulti_bar->MaxBar();
	}
	else
	{
		// UI
		if (ulti_bar)
			ulti_bar->UpdateBar(collective_ultimate_charge / max_ultimate_charge);
	}
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

	// UI
	if(ulti_bar)
		ulti_bar->GetComponent<UltiBar>()->UpdateBar(collective_ultimate_charge);
}

void PlayerManager::CancelUltimate()
{
	Time::SetScaleTime(1.0f);
	ultimate_is_active = false;

	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->OnUltimateDeactivation(1/ultimate_effect_value);
	}
}
