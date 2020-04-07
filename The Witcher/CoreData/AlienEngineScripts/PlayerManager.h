#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API PlayerManager : public Alien {

public:

	PlayerManager();
	virtual ~PlayerManager();
	
	void Start();
	void Update();

	void OnPlayerDead(PlayerController* dead_player);
	void OnPlayerRevive(PlayerController* revived_player);

	uint collective_ultimate_charge = 0;
	uint ultimate_buttons_pressed = 0;

	std::vector<PlayerController*> players;
	std::vector<PlayerController*> players_dead;

private:
	void ActivateUltimate();
	void CancelUltimate();
};

ALIEN_FACTORY PlayerManager* CreatePlayerManager() {
	PlayerManager* alien = new PlayerManager();
	// To show in inspector here

	return alien;
} 
