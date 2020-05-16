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
	void OnEnemyDying(GameObject* enemy);

private: 
	float Normalize(float value, float min, float max)
	{
		return (value - min) / (max - min); 
	}

	void SetEnemyValue(Enemy* enemy, const char* value_name, float variance); 
	void SetNewLeader(); 
	void PositionSphere(); 
	void EncloseEnemies(); 
	void SetGroupStrength(); 
	void SetEnemiesStats(); 
	void ToggleIndicator(bool show); 
	int GetAliveEnemyCount() const; 
	

private:
	Sphere enclosing_sphere;
	float3 mid_point;
	float3 icon_offset; 
	float current_group_strength = 1.f;
	float current_respawn_time = 0.f; 
	bool leaderAlive = false; 

public:
	GameObject* leader = nullptr;
	GameObject* leader_indicator = nullptr; 
	float effect_offset_Y = 2.f; 
	float max_group_strength = 2.f;
	float leader_respawn_time = 0.5f; 
	int leader_respawn_prob = 50; 
	StatVariances stat_variances;

};

ALIEN_FACTORY EnemyGroupLogic* CreateEnemyGroupLogic() {
	EnemyGroupLogic* group = new EnemyGroupLogic();
	// To show in inspector here

	SHOW_TEXT("Leader");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader_indicator);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(group->leader_respawn_prob);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_respawn_time);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->effect_offset_Y);

	SHOW_TEXT("Group");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->max_group_strength);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.health);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.damage);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.agility);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.attack_speed);

	
	
	return group;
}
