#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class InGame_UI;
class UltiBar;

class ALIEN_ENGINE_API PlayerManager : public Alien {

public:

	PlayerManager();
	virtual ~PlayerManager();
	
	void Start();
	void Update();

	void OnPlayerDead(PlayerController* dead_player);
	void OnPlayerRevive(PlayerController* revived_player);
	void BlockInput(bool block);

	std::vector<PlayerController*> players;
	std::vector<PlayerController*> players_dead;

private:

	InGame_UI* in_game_ui = nullptr;

};

ALIEN_FACTORY PlayerManager* CreatePlayerManager() {
	PlayerManager* alien = new PlayerManager();

	// To show in inspector here

	return alien;
} 
