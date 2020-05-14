#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;
class ALIEN_ENGINE_API EnemyGroupLogic : public Alien {
public:

	EnemyGroupLogic() {}
	virtual ~EnemyGroupLogic() {}

	void Start();
	void Update();

	void OnDrawGizmosSelected();

private:
	Sphere enclosing_sphere;
	float3 mid_point;
	float current_group_strength = 1.f;

public:
	GameObject* leader = nullptr;
	float min_group_strength = 1.f;
	float max_group_strength = 10.f;
	float group_stength_factor = 5.f; 

};

ALIEN_FACTORY EnemyGroupLogic* CreateEnemyGroupLogic() {
	EnemyGroupLogic* group = new EnemyGroupLogic();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->min_group_strength);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->max_group_strength);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->group_stength_factor);

	return group;
}
