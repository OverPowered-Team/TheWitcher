#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Boss.h"

class ALIEN_ENGINE_API CiriOriginal : public Boss {
public:

public:
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero()) override;

	void SetActionProbabilities() override;
	void SetActionVariables() override;
	bool IsOnAction() override;

	void LaunchAction() override;

	ActionState UpdateAction() override;

	void EndAction(GameObject* go_ended) override;

	void OnAnimationEnd(const char* name);

	void OnDrawGizmosSelected();

};

ALIEN_FACTORY CiriOriginal* CreateCiriOriginal() {
	CiriOriginal* Ciri_Original = new CiriOriginal();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Boss::BossState, Ciri_Original->state);

	return Ciri_Original;
}
