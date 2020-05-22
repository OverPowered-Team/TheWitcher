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

void SteeringAvoid::Update()
{
	std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), avoidRange);

	for (int i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i]->game_object_attached != game_object && strcmp(colliders[i]->game_object_attached->GetTag(), "Enemy") == 0)
		{
			float3 direction = (colliders[i]->game_object_attached->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
			enemyScript->character_ctrl->Move(-direction * enemyScript->stats["Agility"].GetValue() * Time::GetDT() * Time::GetScaleTime());
		}
	}
}

void SteeringAvoid::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), avoidRange, Color::Cyan());
}