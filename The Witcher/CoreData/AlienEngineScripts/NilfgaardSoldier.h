#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"
#include "BlockerObstacle.h"

class MusicController;

enum(NilfgaardSoldierState,
	NONE = -1,
	IDLE,
	MOVE,
	ATTACK,
	GUARD,
	AUXILIAR,
	STUNNED,
	HIT,
	DYING,
	DEAD
	);

class NilfgaardSoldier : public Enemy {
public:
	enum(NilfgaardType,
		NONE = -1,
		SWORD = 0,
		ARCHER = 1,
		SWORD_SHIELD = 2,
		MAX_NILFGAARD
		);

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}

	void StartEnemy() override;
	void SetStats(const char* json) override;

	void Stun(float time) override;
	bool IsDead() override;
	bool IsDying() override;
	void SetState(const char* state_str) override;

	void OnDeathHit();
	void CheckDistance();

	bool IsRangeEnemy();

	void RotateSoldier();

	void PlaySFX(const char* sfx_name);

	void OnAnimationEnd(const char* name) override;
	void OnTriggerEnter(ComponentCollider* collider) {};
	bool IsState(const char* state_str);
public:
	NilfgaardType nilf_type = NilfgaardType::NONE;
	NilfgaardSoldierState state = NilfgaardSoldierState::NONE;
	MusicController* m_controller = nullptr;
};


