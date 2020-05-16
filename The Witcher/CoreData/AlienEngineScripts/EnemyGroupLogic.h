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

	struct LeaderMultiplier
	{
		float group_strength = 10.f;
		float leader_strength = 20.f; 
	};

	// Debug
	struct debugData
	{
		float health_percent;
		float damage_percent;
		float agility_percent;
		float attack_speed_percent;

		float health_percent_leader;
		float damage_percent_leader;
		float agility_percent_leader;
		float attack_speed_percent_leader;
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
		if (value > max)
			value = max;
		else if (value < min)
			value = min; 
		return (value - min) / (max - min); 
	}

	void SetEnemyValue(Enemy* enemy, const char* value_name, float variance, bool isLeader);
	void SetNewLeader(); 
	void PositionSphere(); 
	void EncloseEnemies(); 
	void SetGroupStrength(); 
	void SetEnemiesStats(); 
	void ToggleIndicator(bool show); 
	int GetAliveEnemyCount() const; 
	
	// debug

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
	LeaderMultiplier leader_multiplier; 
	debugData debug_data; 
};

ALIEN_FACTORY EnemyGroupLogic* CreateEnemyGroupLogic() {
	EnemyGroupLogic* group = new EnemyGroupLogic();
	// To show in inspector here

	SHOW_TEXT("Leader");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader_indicator);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->effect_offset_Y);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(group->leader_respawn_prob);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_respawn_time);

	SHOW_SEPARATOR(); 
	SHOW_TEXT("Leader strength percentage gained");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_multiplier.leader_strength);

	SHOW_SEPARATOR();
	SHOW_TEXT("Group strength percentage gained when leader");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_multiplier.group_strength);

	SHOW_SEPARATOR();
	SHOW_TEXT("Group");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->max_group_strength);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.health);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.damage);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.agility);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.attack_speed);

	
	
	return group;
}
