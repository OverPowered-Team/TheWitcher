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
	switch (oscilation_direction)
	{
	case OSCILATION_DIRECTION::X:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().x * Maths::Rad2Deg());
		break;
	}
	case OSCILATION_DIRECTION::Y:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().y * Maths::Rad2Deg());
		break;
	}
	case OSCILATION_DIRECTION::Z:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().z * Maths::Rad2Deg());
		break;
	}
	}
}

void Training_Zone::Update()
{
	float3 rotation_to_add = float3(0,0,0);
	float acceleration_factor = 0.0f;

	if ((cycle_time * 0.5f) >= (Time::GetGameTime() - current_oscilating_time))
	{
		acceleration_factor = (Time::GetGameTime() - current_oscilating_time) / (cycle_time * 0.5f * 0.5f);
	}
	else
	{
		acceleration_factor = (cycle_time - (Time::GetGameTime() - current_oscilating_time)) / (cycle_time * 0.5f * 0.5f);
	}

	switch (oscilation_direction)
	{
	case OSCILATION_DIRECTION::X:
	{
		rotation_to_add = float3((max_oscilation_pos * 2 / cycle_time) * Time::GetDT() * initial_sign * acceleration_factor, 0, 0);
		break;
	}
	case OSCILATION_DIRECTION::Y:
	{
		rotation_to_add = float3(0, (max_oscilation_pos * 2 / cycle_time) * Time::GetDT() * initial_sign * acceleration_factor, 0);
		break;
	}
	case OSCILATION_DIRECTION::Z:
	{
		rotation_to_add = float3(0, 0, (max_oscilation_pos * 2 / cycle_time) * Time::GetDT() * initial_sign * acceleration_factor);
		break;
	}
	}

	transform->AddRotation(rotation_to_add);

	if (Time::GetGameTime() - current_oscilating_time >= cycle_time)
	{
		initial_sign = -initial_sign;
		current_oscilating_time = Time::GetGameTime();
	}
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
