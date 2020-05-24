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

float3 SteeringAvoid::AvoidObstacle()
{
	std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), avoidRange);
	float3 separation_vector = float3::zero();

	for (int i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i]->game_object_attached != game_object && strcmp(colliders[i]->game_object_attached->GetTag(), "Enemy") == 0)
		{
			float3 avoid_direction = colliders[i]->game_object_attached->transform->GetGlobalPosition() - transform->GetGlobalPosition();
			float avoid_distance = avoid_direction.LengthSq();
			if (avoid_distance > avoidRange)
				continue;

			float avoid_force = 1 - (avoid_distance / avoidRange);
			separation_vector += avoid_direction.Normalized() * enemyScript->stats["Acceleration"].GetValue() * math::Abs(avoid_force) * Time::GetDT();
		}
	}

	if (separation_vector.LengthSq() > enemyScript->stats["Acceleration"].GetValue())
		separation_vector = separation_vector.Normalized() * enemyScript->stats["Acceleration"].GetValue();

	return -separation_vector;
}

void SteeringAvoid::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), avoidRange, Color::Cyan());
}