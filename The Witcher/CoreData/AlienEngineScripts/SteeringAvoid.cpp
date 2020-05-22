#include "SteeringAvoid.h"
#include "Enemy.h"

SteeringAvoid::SteeringAvoid() : Alien()
{
}

SteeringAvoid::~SteeringAvoid()
{
}

void SteeringAvoid::Start()
{
	enemyScript = game_object->GetComponent<Enemy>();
}

void SteeringAvoid::AvoidObstacle(float3& direction)
{
	std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), avoidRange);

	for (int i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i]->game_object_attached != game_object && strcmp(colliders[i]->game_object_attached->GetTag(), "Enemy") == 0)
		{
			float3 avoid_direction = colliders[i]->game_object_attached->transform->GetGlobalPosition() - transform->GetGlobalPosition();
			direction -= avoid_direction.Normalized();
		}
	}
}

void SteeringAvoid::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), avoidRange, Color::Cyan());
}