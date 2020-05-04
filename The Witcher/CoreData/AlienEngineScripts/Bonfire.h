#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API Bonfire : public Alien {

public:

	Bonfire();
	virtual ~Bonfire();
	
	void Start();

	void SetBonfireUsed();

public:
	// GameObject
	GameObject* ui_bonfire = nullptr;
	GameObject* checkpoint = nullptr;

	bool has_been_used = false;

private:
	PlayerController* first_player = nullptr;
};

ALIEN_FACTORY Bonfire* CreateBonfire() {
	Bonfire* alien = new Bonfire();
	// To show in inspector here

	return alien;
} 
