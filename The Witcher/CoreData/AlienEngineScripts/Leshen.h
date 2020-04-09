#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

#define TOTAL_PLAYERS 2

class ALIEN_ENGINE_API Leshen : public Enemy {

public:
	enum(ActionType,
		NONE = -1,
		ROOT,
		MELEE,
		CROWS,
		WHIP,
		CLOUD
		);

	//-----------Data to set probabilities-----------//
	float player_distance[TOTAL_PLAYERS];
	float melee_range = 2.0f;
	bool player_rooted[TOTAL_PLAYERS];
	float action_cooldown = 3.0f;
	float time_to_action;
	int times_hitted = 0;
	int phase = 1;
	//-----------Data to set probabilities-----------//

	struct LeshenAction {
		LeshenAction(ActionType _type, float _probability);
		float probability;
		ActionType type = ActionType::NONE;
	};

	std::map<std::string, LeshenAction*> actions;
	LeshenAction* current_action;

public:
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;

	void SetStats(const char* json) override;

	void SetActionProbabilities();
	void SelectAction();
	bool IsOnAction();

	void SetIdleState();
	void SetAttackState();

	bool UpdateAction();

	bool UpdateRootAction();
	bool UpdateMeleeAction();
	bool UpdateCrowsAction();
	bool UpdateWhipAction();
	bool UpdateCloudAction();

	//Phase 2 exclusive
	void HandleHitCount();
};

ALIEN_FACTORY Leshen* CreateLeshen() {
	Leshen* leshen = new Leshen();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, leshen->state);
	SHOW_VOID_FUNCTION(Leshen::ActivateCollider, leshen);
	SHOW_VOID_FUNCTION(Leshen::DeactivateCollider, leshen);

	return leshen;
}
