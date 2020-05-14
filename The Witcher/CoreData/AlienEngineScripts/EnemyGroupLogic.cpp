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
	current_group_strength = (game_object->GetChildren().size() / enclosing_sphere.r) * group_stength_factor;

	LOG("Enemy group with strength WITHOUT CAPPING: %f", current_group_strength);


	// cap the strength between 0.5 and 1.5 (1 as the "default") 
	current_group_strength = 0.5f + ((current_group_strength - min_group_strength) / (max_group_strength - min_group_strength));

	LOG("Enemy group with strength: %f", current_group_strength); 

	// send the strength notifier to each enemy in the group
	for (auto& child : game_object->GetChildren())
	{
		child->GetComponent<Enemy>()->OnGroupStrengthChange(current_group_strength);
	}
}


void EnemyGroupLogic::OnDrawGizmosSelected()
{
	float green = current_group_strength - 0.5f;
	float red = 1 - (current_group_strength - 0.5f);
	Color color = Color(red, green, 0.f);

	Gizmos::DrawWireSphere(enclosing_sphere.pos, enclosing_sphere.r, color);

}
