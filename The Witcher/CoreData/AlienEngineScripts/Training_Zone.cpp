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
	rb = GetComponent<ComponentRigidBody>();

	current_oscilating_time = Time::GetGameTime();
	switch (oscilation_direction)
	{
	case OSCILATION_DIRECTION::X:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().x);
		break;
	}
	case OSCILATION_DIRECTION::Y:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().y);
		break;
	}
	case OSCILATION_DIRECTION::Z:
	{
		max_oscilation_pos = Maths::Abs(transform->GetLocalRotation().ToEulerXYZ().z);
		break;
	}
	}
}

void Training_Zone::Update()
{
	float acceleration_factor = 0.0f;

	if ((cycle_time * 0.5f) >= (Time::GetGameTime() - current_oscilating_time))
	{
		acceleration_factor = (Time::GetGameTime() - current_oscilating_time) / (cycle_time * 0.5f * 0.5f);
	}
	else
	{
		acceleration_factor = (cycle_time - (Time::GetGameTime() - current_oscilating_time)) / (cycle_time * 0.5f * 0.5f);
	}

	float rotation_to_add = (max_oscilation_pos * 2 / cycle_time) * Time::GetDT() * initial_sign * acceleration_factor;

	switch (oscilation_direction)
	{
	case OSCILATION_DIRECTION::X:
	{
		rb->SetRotation(transform->GetGlobalRotation() * Quat::RotateX(rotation_to_add));
		break;
	}
	case OSCILATION_DIRECTION::Y:
	{
		rb->SetRotation(transform->GetGlobalRotation() * Quat::RotateY(rotation_to_add));
		break;
	}
	case OSCILATION_DIRECTION::Z:
	{
		rb->SetRotation(transform->GetGlobalRotation() * Quat::RotateZ(rotation_to_add));
		break;
	}
	}

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
			switch (oscilation_direction)
			{
			case OSCILATION_DIRECTION::X:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, transform->right * initial_sign * push_force);
				break;
			}
			case OSCILATION_DIRECTION::Y:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, float3(0, -initial_sign * push_force, 0));
				break;
			}
			case OSCILATION_DIRECTION::Z:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, transform->forward * initial_sign * push_force);
				break;
			}
			}
			
			break;
		}
		case TYPE::DAMAGE:
		{
			col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do);
			break;
		}
		case TYPE::BOTH:
		{
			switch (oscilation_direction)
			{
			case OSCILATION_DIRECTION::X:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, transform->right * initial_sign * push_force);
				break;
			}
			case OSCILATION_DIRECTION::Y:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, float3(0, -initial_sign * push_force, 0));
				break;
			}
			case OSCILATION_DIRECTION::Z:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, transform->forward * initial_sign * push_force);
				break;
			}
			}
			
			break;
		}
		case TYPE::ANY:
			break;
		}
	}
}
