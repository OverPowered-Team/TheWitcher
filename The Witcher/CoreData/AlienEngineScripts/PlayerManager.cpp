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
}

void PlayerManager::Update()
{

}

void PlayerManager::OnPlayerDead(PlayerController* dead_player)
{
	players_dead.push_back(dead_player);

	if (players_dead.size() == players.size())
	{
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