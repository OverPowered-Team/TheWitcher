#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;
class ALIEN_ENGINE_API EnemyGroupLogic : public Alien {
public:
	struct StatVariances
	{
		float health = 20.f; 
		float damage = 10.f; 
		float agility = 5.f; 
		float attack_speed = 5.f; 
	};
public: 
	EnemyGroupLogic() {}
	virtual ~EnemyGroupLogic() {}

	void Start();
	void Update();
	void OnDrawGizmosSelected();

private: 
	float Normalize(float value, float min, float max)
	{
		return (value - min) / (max - min); 
	}

	void SetEnemyValue(Enemy* enemy, const char* value_name, float variance); 

private:
	Sphere enclosing_sphere;
	float3 mid_point;
	float current_group_strength = 1.f;

public:
	GameObject* leader = nullptr;
	float max_group_strength = 2.f;
	StatVariances stat_variances;

};

ALIEN_FACTORY EnemyGroupLogic* CreateEnemyGroupLogic() {
	EnemyGroupLogic* group = new EnemyGroupLogic();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->max_group_strength);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.health);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.damage);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.agility);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.attack_speed);
	
	return group;
}
