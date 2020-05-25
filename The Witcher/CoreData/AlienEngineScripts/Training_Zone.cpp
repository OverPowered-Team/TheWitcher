#include "Training_Zone.h"
#include "PlayerController.h"

Training_Zone::Training_Zone() : Alien()
{
}

Training_Zone::~Training_Zone()
{
}

void Training_Zone::Start()
{
	current_oscilating_time = Time::GetGameTime();
}

void Training_Zone::Update()
{
	//transform->AddRotation(float3((50 * 2 / cycle_time) * Time::GetDT() * initial_sign, 0, 0));

	//if (Time::GetGameTime() - current_oscilating_time >= cycle_time)
	//{
	//	initial_sign = -initial_sign;
	//	current_oscilating_time = Time::GetGameTime();
	//}
}

void Training_Zone::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player") == 0)
	{
		switch (osciling_type)
		{
		case TYPE::PUSH:
		{
			col->game_object_attached->GetComponent<ComponentRigidBody>()->AddForce(float3(initial_sign * 50, 0, 0));
			break;
		}
		case TYPE::DAMAGE:
		{
			col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do);
			break;
		}
		case TYPE::BOTH:
		{
			col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, float3(initial_sign * 50, 0, 0));
			break;
		}
		case TYPE::ANY:
			break;
		}
	}
}
