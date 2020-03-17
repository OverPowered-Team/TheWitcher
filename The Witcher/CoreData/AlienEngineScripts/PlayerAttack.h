#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerAttack : public Alien {

public:

	PlayerAttack();
	virtual ~PlayerAttack();
	
	void Start();
	void Update();

};

ALIEN_FACTORY PlayerAttack* CreatePlayerAttack() {
	PlayerAttack* alien = new PlayerAttack();
	// To show in inspector here

	return alien;
} 
