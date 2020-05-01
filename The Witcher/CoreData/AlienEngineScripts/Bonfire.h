#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Bonfire : public Alien {

public:

	Bonfire();
	virtual ~Bonfire();
	
	void Start();

	void SetBonfireUsed(int controller_index);

	bool HaveThisPlayerUsedThis(int controller_index);

public:
	// GameObject
	GameObject* ui_bonfire = nullptr;
	GameObject* checkpoint = nullptr;

	bool is_active = true;

private:
	int bonfire_used = 0;
	int first_player_index = -1;
};

ALIEN_FACTORY Bonfire* CreateBonfire() {
	Bonfire* alien = new Bonfire();
	// To show in inspector here

	return alien;
} 
