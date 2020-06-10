#include "PlayerController.h"
#include "PlayerManager.h"
#include "InGame_UI.h"
#include "Scores_Data.h"
#include "UltiBar.h"

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

	in_game_ui = GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetComponent<InGame_UI>();
	ultibar = GameObject::FindWithName("Ulti_Bar")->GetComponent<UltiBar>();

	if (Scores_Data::player1_relics.size() > 0)
		players[0]->relics = Scores_Data::player1_relics;
	if (Scores_Data::player2_relics.size() > 0)
		players[1]->relics = Scores_Data::player1_relics;
}

void PlayerManager::Update()
{
	int num_pressed = 0;
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		if (Input::GetControllerButtonRepeat((*it)->controller_index, (*it)->controller_ultimate) || Input::GetKeyRepeat((*it)->keyboard_ultimate))
		{
			num_pressed++;
		}
	}
	if (num_pressed == players.size() && collective_ultimate_charge >= max_ultimate_charge)
	{
		ActivateUltimate();
	}
}

void PlayerManager::OnPlayerDead(PlayerController* dead_player)
{
	players_dead.push_back(dead_player);

	if (players_dead.size() == players.size())
	{
		Scores_Data::player1_kills = players[0]->player_data.type_kills;
		Scores_Data::player2_kills = players[1]->player_data.type_kills;
		Scores_Data::player1_relics = players[0]->relics;
		Scores_Data::player2_relics = players[1]->relics;
		Scores_Data::dead = true;
		in_game_ui->YouDied(); 
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

void PlayerManager::BlockInput(bool block)
{
	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->input_blocked = block;
;	}
}

void PlayerManager::IncreaseUltimateCharge(uint value)
{
	if (ultimate_is_active)
		return;

	collective_ultimate_charge += value;

	if (collective_ultimate_charge >= max_ultimate_charge)
	{
		collective_ultimate_charge = max_ultimate_charge;
	}
	
	// UI
	in_game_ui->StartLerpParticleUltibar(float3(0, -38.f, 0));
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
	ultibar->GetComponent<UltiBar>()->UpdateBar(collective_ultimate_charge);
	in_game_ui->ShowUltiFilter(true);
}

void PlayerManager::CancelUltimate()
{
	Time::SetScaleTime(1.0f);
	ultimate_is_active = false;

	for (std::vector<PlayerController*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it)->OnUltimateDeactivation(1/ultimate_effect_value);
	}

	in_game_ui->ShowUltiFilter(false);
}
