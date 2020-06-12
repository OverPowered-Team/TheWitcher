#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Boss.h"

class CiriFightController;

class ALIEN_ENGINE_API CiriOriginal : public Boss {
public:
	CiriFightController* fight_controller = nullptr;
	Prefab rock;
	float rock_force = 0.6f;
	float scream_range = 20.0f;
	float scream_force = 7.0f;
public:
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, float3 knock_back = float3::zero()) override;

	void SetActionProbabilities() override;
	void SetActionVariables() override;
	bool IsOnAction() override;

	void LaunchAction() override;
	void Scream();

	void LaunchRockAction();
	void LaunchScreamAction();

	ActionState UpdateAction() override;

	void EndAction(GameObject* go_ended) override;

	void OnAnimationEnd(const char* name);

	void OnDrawGizmosSelected();

	void SetStats(const char* json) override;

};

ALIEN_FACTORY CiriOriginal* CreateCiriOriginal() {
	CiriOriginal* cirioriginal = new CiriOriginal();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Boss::BossState, cirioriginal->state);
	SHOW_IN_INSPECTOR_AS_PREFAB(cirioriginal->rock);
	SHOW_VOID_FUNCTION(CiriOriginal::Scream, cirioriginal);

	return cirioriginal;
}
