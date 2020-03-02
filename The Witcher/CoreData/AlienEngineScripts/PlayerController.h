#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

struct PlayerData {
	float speed = 20;
	bool attacking = false;
	// dmg, deff, lvl bla bla
};

class ALIEN_ENGINE_API PlayerController : public Alien {

public:

	PlayerController();
	virtual ~PlayerController();
	
	void Start();
	void Update();

public:

	PlayerData playerData;
	ComponentAnimator* animator = nullptr;
};

ALIEN_FACTORY PlayerController* CreatePlayerController() {
	PlayerController* player = new PlayerController();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->playerData.speed);
	return player;
} 
