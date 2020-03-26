#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
public:

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}

	void StartEnemy() override;
	void SetStats(const char* json) override;
	void UpdateEnemy() override;
	void CleaningUp() override;

public:


};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* alien = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, alien->state);
	return alien;
}

ALIEN_FACTORY void DestroyNilfgaardSoldier(void* pointer) {
	delete pointer;
	pointer = nullptr;
}


