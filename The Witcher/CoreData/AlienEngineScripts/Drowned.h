#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API Drowned : public Enemy {
public:
	enum(DrownedType,
		NONE = -1,
		RANGE = 0,
		GRAB = 1);

	enum(DrownedState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		GETOFF,
		HIDE,
		HIT,
		DYING,
		DEATH);

	Drowned();
	virtual ~Drowned();
	
	void StartEnemy() override;
	void SetStats(const char* json) override;

public:
	DrownedState state = DrownedState::NONE;
	DrownedType drowned_type = DrownedType::NONE;
};

ALIEN_FACTORY Drowned* CreateDrowned() {
	Drowned* drowned = new Drowned();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedType, drowned->drowned_type);

	return drowned;
} 
