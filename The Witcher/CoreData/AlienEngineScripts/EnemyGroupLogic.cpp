#include "EnemyGroupLogic.h"
#include "Enemy.h"

void EnemyGroupLogic::Start()
{
	enclosing_sphere = math::Sphere();

	// position the sphere at the middle point between the enemies
	mid_point = float3(0.f, 0.f, 0.f);
	for (auto& child : game_object->GetChildren())
	{
		mid_point += child->GetComponent<ComponentTransform>()->GetGlobalPosition();
	}
	mid_point /= game_object->GetChildren().size();
	enclosing_sphere.pos = mid_point;
}

void EnemyGroupLogic::Update()
{
	if (game_object->GetChildren().empty())
		return; 

	// position the sphere at the middle point between the enemies
	mid_point = float3(0.f, 0.f, 0.f);
	for (auto& child : game_object->GetChildren())
	{
		mid_point += child->GetComponent<ComponentTransform>()->GetGlobalPosition();
	}
	mid_point /= game_object->GetChildren().size();
	enclosing_sphere.pos = mid_point;

	// enclose all child enemies in a sphere
	enclosing_sphere.r = 0.f; 
	for (auto& child : game_object->GetChildren())
	{
		enclosing_sphere.Enclose(child->GetComponent<ComponentTransform>()->GetGlobalPosition());
	}

	// the closer and the more enemies there are, the higher the group strength
	current_group_strength = (game_object->GetChildren().size() / enclosing_sphere.r);

	LOG("Enemy group with strength WITHOUT CAPPING: %f", current_group_strength);


	// cap the strength between 0 and 1  
	current_group_strength = Normalize(current_group_strength, 0.f, max_group_strength); 

	LOG("Enemy group with strength: %f", current_group_strength); 

	// change each enemy's stats
	for (auto& child : game_object->GetChildren())
	{
		auto enemy = child->GetComponent<Enemy>();
		
		SetEnemyValue(enemy, "Health", stat_variances.health);
		SetEnemyValue(enemy, "Damage", stat_variances.damage);
		SetEnemyValue(enemy, "Agility", stat_variances.agility);
		SetEnemyValue(enemy, "AttacKSpeed", stat_variances.attack_speed);

	}
}

void EnemyGroupLogic::SetEnemyValue(Enemy* enemy, const char* value_name, float variance)
{
	float start = -variance / 2.f; // eg if it is a 20% variance, it starts at -10%
	float percentage = start + current_group_strength * variance; // eg -10 + 0.75 * 20 = 5, which means 0.75 group strength equals to +5% in this stat
	float multiplier = percentage / 100.f; // eg 0,05
	float base_value = enemy->stats[value_name].GetBaseValue(); // eg 100 health
	float result_value = base_value + base_value * multiplier; // eg 100 + 100 * 0,05

	enemy->stats[value_name].SetCurrentStat(result_value, false); 

	// DEBUG
	LOG("Due to group dynamics, an enemy's %s with base value %f is multiplied by %f and is now theoretically %f and practically %f", 
		value_name, base_value, multiplier, result_value, enemy->stats[value_name].GetValue()); 
}


void EnemyGroupLogic::OnDrawGizmosSelected()
{
	float green = current_group_strength; 
	float red = 1 - current_group_strength;
	Color color = Color(red, green, 0.f);

	Gizmos::DrawWireSphere(enclosing_sphere.pos, enclosing_sphere.r, color);

}
