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
		STUNNED,
		HIT,
		DYING,
		DEAD);

	Drowned();
	virtual ~Drowned();
	
	void StartEnemy() override;
	void SetStats(const char* json) override;

	float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero()) override;

	void Stun(float time) override;
	bool IsDead() override;

public:
	DrownedState state = DrownedState::NONE;
	DrownedType drowned_type = DrownedType::NONE;
	bool is_hide = true;
};

ALIEN_FACTORY Drowned* CreateDrowned() {
	Drowned* drowned = new Drowned();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedType, drowned->drowned_type);

	return drowned;
} 
