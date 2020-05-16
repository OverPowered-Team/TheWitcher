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
	if (leader)
	{
		leaderAlive = true; 
		ToggleIndicator(true);
	}
	
	icon_offset = float3(0, effect_offset_Y, 0); 
}

void EnemyGroupLogic::Update()
{
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
}

void EnemyGroupLogic::SetEnemyValue(Enemy* enemy, const char* value_name, float variance)
{
	float start = -variance / 2.f;  
	float multiplier = (start + current_group_strength * variance) * 0.01f; 
	float base_value = enemy->stats[value_name].GetBaseValue();  
	float result_value = base_value + base_value * multiplier;  

	enemy->stats[value_name].SetCurrentStat(result_value); 

	// DEBUG
/*	LOG("Due to group dynamics, an enemy's %s with base value %f is multiplied by %f and is now theoretically %f and practically %f", 
		value_name, base_value, multiplier, result_value, enemy->stats[value_name].GetValue());*/ 
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

		SetEnemyValue(enemy, "Health", stat_variances.health);
		SetEnemyValue(enemy, "Damage", stat_variances.damage);
		SetEnemyValue(enemy, "Agility", stat_variances.agility);
		SetEnemyValue(enemy, "AttacKSpeed", stat_variances.attack_speed);
	}
}

void EnemyGroupLogic::ToggleIndicator(bool show)
{
	for (auto& child : leader_indicator->GetChildren())
		child->GetComponent<ComponentMesh>()->SetEnable(show);  // won't work ?
}

int EnemyGroupLogic::GetAliveEnemyCount() const
{
	int i = 0; 
	for (auto& child : game_object->GetChild("Group_Enemies")->GetChildren())
		if (child->GetComponent<Enemy>()->IsDead() == false)
			++i; 
	return i; 
}

