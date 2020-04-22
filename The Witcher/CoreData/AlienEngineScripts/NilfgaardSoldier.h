#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
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
	void UpdateEnemy() override;

	void SetStats(const char* json) override;
	//void Move(float3 direction) override;
	//void Attack() override;
	float GetDamaged(float dmg, PlayerController* player) override;
	void Action() override;
	void Block();
	void Flee(float3 direction);

	void ShootAttack();
	Quat RotateArrow();
	void OnDeathHit();

	void OnAnimationEnd(const char* name) override;
	void OnTriggerEnter(ComponentCollider* collider) override;

public:
	Prefab arrow;
	NilfgaardType nilf_type = NilfgaardType::NONE;
	float block_time = 2.0f;
	float block_attack_time = 3.0f;
	int max_break_shield_attack = 5;

private:
	float current_time = 0.0f;
	bool has_been_attacked = false;
	bool is_blocked = false;
	int break_shield_attack = 0;
};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* nilfgaard = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->arrow);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->head_prefab);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(nilfgaard->head_position);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ShootAttack, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->block_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->block_attack_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_INT(nilfgaard->max_break_shield_attack);


	return nilfgaard;
}


