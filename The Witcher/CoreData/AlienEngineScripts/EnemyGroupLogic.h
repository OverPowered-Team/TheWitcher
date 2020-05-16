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
	void InputDebugValue(std::string name, float base_value, float value, bool isLeader); 

private:
	Sphere enclosing_sphere;
	float3 mid_point;
	float3 icon_offset; 
	float current_group_strength = 1.f;
	float current_respawn_time = 0.f; 
	bool leaderAlive = false; 

	// Debug
	bool debug_this_frame = false; 
	float current_debug_time = 0.f;

public:
	GameObject* leader = nullptr;
	GameObject* leader_indicator = nullptr; 
	float indicator_offset_Y = 2.f; 
	float max_group_strength = 2.f;
	float leader_respawn_time = 0.5f; 
	int leader_respawn_prob = 50; 
	StatVariances stat_variances;
	LeaderMultiplier leader_multiplier; 

	// Debug
	bool debug = false;
	float debug_time = 0.f; 
	std::map<std::string, std::string> debugDataMap; // hp, dmg, ag, att, l_hp, l_dmg, l_ag, l_att
};

ALIEN_FACTORY EnemyGroupLogic* CreateEnemyGroupLogic() {
	EnemyGroupLogic* group = new EnemyGroupLogic();
	// To show in inspector here

	SHOW_TEXT("Leader");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(group->leader_indicator);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->indicator_offset_Y);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(group->leader_respawn_prob);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_respawn_time);

	SHOW_SPACING(); 
	SHOW_SEPARATOR(); 
	SHOW_TEXT("Leader strength percentage gained");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_multiplier.leader_strength);

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("Group strength percentage gained when leader");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->leader_multiplier.group_strength);

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("Group strength = n enemies / enclosing sphere radius");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->max_group_strength);

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("Group non-leaders stats percentage gained");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.health);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.damage);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.agility);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->stat_variances.attack_speed);

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("Debug stat change percentages in the console");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(group->debug); // todo the rest if this works
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(group->debug_time);

	
	return group;
}
