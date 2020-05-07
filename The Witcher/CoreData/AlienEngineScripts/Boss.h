#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

#define TOTAL_PLAYERS 2

class MusicController;

class ALIEN_ENGINE_API Boss : public Enemy {

public:
	enum(BossState,
		NONE = -1,
		IDLE,
		ATTACK,
		DYING,
		HIT,
		DEAD
		);

	enum(ActionType,
		NONE = -1,
		ROOT,
		MELEE,
		CROWS,
		CLOUD,
		DASH,
		COMBO,
		SCREAM,
		MINISCREAM,
		ROCKTHROW,
		AFK
		);

	enum(ActionState,
		NONE = -1,
		LAUNCH,
		UPDATING,
		ENDED
		);

	BossState state = BossState::NONE;

	struct BossAction {
		BossAction(ActionType _type, float _probability);
		float probability = 0.0f;
		ActionType type = ActionType::NONE;
		ActionState state = ActionState::NONE;
	};

	std::map<std::string, BossAction*> actions;
	BossAction* current_action;

	GameObject* meshes = nullptr;

	float action_time = 0;
	float action_cooldown = 1.5f;
	float time_to_action;
	float time_to_rotate = 1.0f;
	float rotate_time = 0.0f;

	float player_distance[TOTAL_PLAYERS];
	MusicController* m_controller = nullptr;
public:
	virtual void StartEnemy() override;
	virtual void UpdateEnemy() override;
	virtual void CleanUpEnemy() override;
	virtual float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero()) override;

	virtual void SetActionVariables();
	virtual void SetActionProbabilities();
	virtual void SelectAction();
	virtual void SetIdleState();
	virtual void SetAttackState();
	virtual bool IsOnAction();

	virtual void LaunchAction();
	virtual ActionState UpdateAction();
	virtual void EndAction(GameObject* go_ended);
	virtual float GetDamaged(float dmg, float3 knock_back = float3::zero()) override;

	virtual void SetStats(const char* json) override;

	void OrientToPlayer(int target);
	void OrientToPlayerWithoutSlerp(int target);

};

ALIEN_FACTORY Boss* CreateBoss() {
	Boss* alien = new Boss();
	// To show in inspector here

	return alien;
} 
