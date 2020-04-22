#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

#define TOTAL_PLAYERS 2
#define TOTAL_CROWS 10

class ALIEN_ENGINE_API Leshen : public Enemy {

public:
	enum(ActionType,
		NONE = -1,
		ROOT,
		MELEE,
		CROWS,
		CLOUD
		);

	enum(ActionState,
		NONE = -1,
		LAUNCH,
		UPDATING,
		ENDED
		);

	//-----------Data to set probabilities-----------//
	float player_distance[TOTAL_PLAYERS];
	float melee_range = 2.0f;
	bool player_rooted[TOTAL_PLAYERS];
	float action_cooldown = 3.0f;
	float time_to_action;
	int times_hitted = 0;
	//int phase = 1;
	float action_time = 0;
	//-----------Data to set probabilities-----------//

	struct LeshenAction {
		LeshenAction(ActionType _type, float _probability);
		float probability = 0.0f;
		ActionType type = ActionType::NONE;
		ActionState state = ActionState::NONE;
	};

	std::map<std::string, LeshenAction*> actions;
	LeshenAction* current_action;

	Prefab root_prefab;
	GameObject* root_1 = nullptr;
	GameObject* root_2 = nullptr;

	Prefab crow_prefab;
	GameObject* crows = nullptr;

	float3 direction;
	float direction_time = 0.0f;
	float switch_direction_time = 3.0f;
	float times_switched = 0;
	float total_switch_times = 5;
	float speed = 0.05f;

	int crows_target = 0;

	int rand_x = 0;
	int rand_z = 0;


public:
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, PlayerController* player) override;

	void SetStats(const char* json) override;

	void SetActionProbabilities();
	void SelectAction();
	bool IsOnAction();

	void FinishAttack();

	void SetIdleState();
	void SetAttackState();

	void LaunchAction();

	void LaunchRootAction();
	void LaunchMeleeAction();
	void LaunchCrowsAction();
	void LaunchCloudAction();

	bool UpdateAction();

	ActionState UpdateRootAction();
	ActionState UpdateMeleeAction();
	ActionState UpdateCrowsAction();
	ActionState UpdateCloudAction();

	void EndRootAction(GameObject* root);
	void EndMeleeAction();
	void EndCrowsAction(GameObject* crow);
	void EndCloudAction();

	void CreateRoot();

	void SetActionVariables();
	//void ChangePhase();

	//Phase 2 exclusive
	void HandleHitCount();

	void SetRandomDirection();
};

ALIEN_FACTORY Leshen* CreateLeshen() {
	Leshen* leshen = new Leshen();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, leshen->state);
	SHOW_VOID_FUNCTION(Leshen::ActivateCollider, leshen);
	SHOW_VOID_FUNCTION(Leshen::DeactivateCollider, leshen);
	SHOW_IN_INSPECTOR_AS_PREFAB(leshen->root_prefab);
	SHOW_IN_INSPECTOR_AS_PREFAB(leshen->crow_prefab);

	return leshen;
}
