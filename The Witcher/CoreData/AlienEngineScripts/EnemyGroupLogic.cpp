#include "EnemyGroupLogic.h"
#include "Enemy.h"
#include "ParticlePool.h"

void EnemyGroupLogic::Start()
{
	// Sphere
	enclosing_sphere = math::Sphere();

	// position the sphere at the middle point between the enemies
	PositionSphere(); 

	// Leader
	icon_offset = float3(0, indicator_offset_Y, 0);

	if (leader)
	{
		leader_indicator->transform->SetGlobalPosition(leader->transform->GetGlobalPosition() + icon_offset);
		leaderAlive = true; 
		ToggleIndicator(true);
	}
	else
		ToggleIndicator(false);
	
	// Debug
	debugDataMap = // clean up? 
	{
		{ "Health", "value"}, 
		{ "Damage", "value"},
		{ "Agility", "value"},
		{ "AttackSpeed", "value"},
		{ "Health Leader", "value"},
		{ "Damage Leader", "value"},
		{ "Agility Leader", "value"},
		{ "AttackSpeed Leader", "value"}
	}; 
}

void EnemyGroupLogic::Update()
{
	// debug
	if (debug)
	{
		if ((current_debug_time += Time::GetDT()) >= debug_time)
		{
			current_debug_time = 0.0f;
			debug_this_frame = true; 
		}
	}

	// Leader effect
	if (leaderAlive)
		leader_indicator->transform->SetGlobalPosition(leader->transform->GetGlobalPosition() + icon_offset);

	// position the sphere at the middle point between the enemies
	PositionSphere(); 

	// enclose all child enemies in a sphere
	EncloseEnemies(); 

	// Compute group strength
	SetGroupStrength(); 

	// change each enemy's stats
	SetEnemiesStats(); 

	// Debug
	debug_this_frame = false;
	
}

void EnemyGroupLogic::SetEnemyValue(Enemy* enemy, const char* value_name, float variance, bool isLeader)
{
	float start = -variance / 2.f;  
	float multiplier = (start + current_group_strength * variance) * 0.01f; 
	float base_value = enemy->stats[value_name].GetBaseValue();  
	float result_value = base_value + base_value * multiplier;  

	if (isLeader)
	{
		float increment = result_value * leader_multiplier.leader_strength * 0.01f; 
		result_value += increment;
	}
	
	enemy->stats[value_name].SetCurrentStat(result_value); 

	if(debug_this_frame)
		InputDebugValue(value_name, enemy->stats[value_name].GetBaseValue(), enemy->stats[value_name].GetValue(), isLeader);
}

void EnemyGroupLogic::InputDebugValue(std::string name, float base_value, float value, bool isLeader)
{
	float percentage = value - base_value; 
	if (isLeader)
		name.append(" Leader"); 
	std::string result = "+" + std::to_string(percentage);

	debugDataMap.at(name) = result; 
	LOG("====================== Enemy with stat %s gets a %s percentage from group tactics ======================", 
		name.c_str(), result.c_str()); 
}


void EnemyGroupLogic::OnDrawGizmosSelected()
{
	float green = current_group_strength; 
	float red = 1 - current_group_strength;
	Color color = Color(red, green, 0.f);

	Gizmos::DrawWireSphere(enclosing_sphere.pos, enclosing_sphere.r, color);

}

void EnemyGroupLogic::OnEnemyDying(GameObject* enemy)
{
	bool leaderKilled = false; 

	if (leaderAlive && (enemy == leader)) // for safety
	{
		leader = nullptr;
		leaderAlive = false;

		// for the sake of the next lines
		leaderKilled = true; 
	}
	
	int enemy_count = GetAliveEnemyCount(); 

	if (enemy_count == 0) // no enemies alive
		game_object->ToDelete();
	else if (enemy_count == 1) // disable indicator, no leader will be elected from now on
		ToggleIndicator(false);
	else if (enemy_count > 1) // multiple enemies alive --> set new leader ONLY IF the dead one was the leader
		if (leaderKilled)
			SetNewLeader();
}


void EnemyGroupLogic::SetNewLeader()
{
	// have a chance to select new leader (otherwise stop particle system)
	if (Random::GetRandomIntBetweenTwo(0, 100) <= leader_respawn_prob)
	{
		// Search for a new leader --> CAUTION: the current one still exists! Don't take it into account (also dead ones)
		std::vector<GameObject*> alive_enemies_no_leader; 
		for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
			if ((child != leader) && (child->GetComponent<Enemy>()->IsDead() == false))
				alive_enemies_no_leader.push_back(child);

		int index = Random::GetRandomIntBetweenTwo(0, alive_enemies_no_leader.size() - 1);
		leader = alive_enemies_no_leader.at(index);
		leader_indicator->transform->SetGlobalPosition(leader->transform->GetGlobalPosition() + icon_offset);
		leaderAlive = true; 


		// Effect
		ToggleIndicator(true);

	}
	else
		ToggleIndicator(false);

}

void EnemyGroupLogic::PositionSphere()
{
	mid_point = float3(0.f, 0.f, 0.f);
	for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
	{
		if(child->GetComponent<Enemy>()->IsDead() == false)
			mid_point += child->GetComponent<ComponentTransform>()->GetGlobalPosition();
	}
		
	mid_point /= GetAliveEnemyCount(); 
	enclosing_sphere.pos = mid_point;
}

void EnemyGroupLogic::EncloseEnemies()
{
	enclosing_sphere.r = 0.f;
	for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
	{
		if (child->GetComponent<Enemy>()->IsDead() == false)
			enclosing_sphere.Enclose(child->GetComponent<ComponentTransform>()->GetGlobalPosition());
	}
}

void EnemyGroupLogic::SetGroupStrength()
{
	// the closer and the more enemies there are, the higher the group strength
	current_group_strength = GetAliveEnemyCount() / enclosing_sphere.r;

	// extra % when leader alive
	if (leaderAlive)
	{
		float increment = current_group_strength * leader_multiplier.group_strength * 0.01f; 
		current_group_strength += increment;
	}
	

	// cap the strength between 0 and 1  
	current_group_strength = Normalize(current_group_strength, 0.f, max_group_strength);
}

void EnemyGroupLogic::SetEnemiesStats()
{
	for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
	{
		auto enemy = child->GetComponent<Enemy>();
		if (enemy->IsDead() == true)
			continue; 

		bool isLeader = ((leaderAlive) && (child == leader)) ? true : false; 

		SetEnemyValue(enemy, "Health", stat_variances.health, isLeader);
		SetEnemyValue(enemy, "Damage", stat_variances.damage, isLeader);
		SetEnemyValue(enemy, "Agility", stat_variances.agility, isLeader);
		SetEnemyValue(enemy, "AttackSpeed", stat_variances.attack_speed, isLeader);
	}
}

void EnemyGroupLogic::ToggleIndicator(bool show)
{
	for (auto& child : leader_indicator->GetChildren())
		child->GetComponent<ComponentMesh>()->SetEnable(show);  
}

int EnemyGroupLogic::GetAliveEnemyCount() const
{
	int i = 0; 
	for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
		if (child->GetComponent<Enemy>()->IsDead() == false)
			++i; 
	return i; 
}

