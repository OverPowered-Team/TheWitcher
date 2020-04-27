#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Boss.h"

#define TOTAL_PLAYERS 2

class ALIEN_ENGINE_API Leshen : public Boss {

public:
	//-----------Data to set probabilities-----------//
	bool player_rooted[TOTAL_PLAYERS];
	int times_hitted = 0;
	//int phase = 1;
	//-----------Data to set probabilities-----------//

	Prefab root_prefab;
	GameObject* root_1 = nullptr;
	GameObject* root_2 = nullptr;

	Prefab crow_prefab;
	GameObject* crows = nullptr;
	GameObject* meshes = nullptr;

	float3 direction;
	float melee_range = 3.0f;
	float direction_time = 0.0f;
	float switch_direction_time = 3.0f;
	float times_switched = 0;
	float total_switch_times = 5;
	float speed = 3.0f;

	int crows_target = 0;

	int rand_x = 0;
	int rand_z = 0;


public:
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, PlayerController* player) override;

	void SetActionProbabilities() override;
	void SetActionVariables() override;
	bool IsOnAction() override;

	void LaunchAction() override; 

	void LaunchRootAction();
	void LaunchMeleeAction();
	void LaunchCrowsAction();
	void LaunchCloudAction();

	ActionState UpdateAction() override;

	ActionState UpdateRootAction();
	ActionState UpdateMeleeAction();
	ActionState UpdateCrowsAction();
	ActionState UpdateCloudAction();

	void EndAction(GameObject* go_ended) override;

	void EndRootAction(GameObject* root);
	void EndMeleeAction();
	void EndCrowsAction();
	void EndCloudAction();

	//Leshen things
	void HandleHitCount();
	void SetRandomDirection();

	void OnAnimationEnd(const char* name);
};

ALIEN_FACTORY Leshen* CreateLeshen() {
	Leshen* leshen = new Leshen();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, leshen->state);
	SHOW_VOID_FUNCTION(Leshen::ActivateCollider, leshen);
	SHOW_VOID_FUNCTION(Leshen::DeactivateCollider, leshen);
	SHOW_IN_INSPECTOR_AS_PREFAB(leshen->root_prefab);
	SHOW_IN_INSPECTOR_AS_PREFAB(leshen->crow_prefab);
	SHOW_VOID_FUNCTION(Leshen::LaunchRootAction, leshen);
	SHOW_VOID_FUNCTION(Leshen::LaunchCrowsAction, leshen);

	return leshen;
}
