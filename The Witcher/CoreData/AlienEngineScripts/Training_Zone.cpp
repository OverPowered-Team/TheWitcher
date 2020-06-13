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
	Invoke(std::bind(&Training_Zone::Init, this), 10);
}

void Training_Zone::Update()
{
	if (!has_started) { return; }

	if (!Time::IsGamePaused())
	{
		internal_timer += Time::GetDT();

		if (time_paused != 0.0f)
		{
			current_oscilating_time += time_paused;
			internal_timer += time_paused;
			time_paused = 0.0f;
		}
	}
	else
	{
		time_paused += Time::GetDT();

		return;
	}

	float acceleration_factor = 0.0f;

	if ((cycle_time * 0.5f) > (internal_timer - current_oscilating_time))
	{
		acceleration_factor = (internal_timer - current_oscilating_time) / (cycle_time * 0.5f * 0.5f);
	}
	else
	{
		acceleration_factor = (cycle_time - (internal_timer - current_oscilating_time)) / (cycle_time * 0.5f * 0.5f);
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

	if (internal_timer - current_oscilating_time >= cycle_time)
	{
		initial_sign = -initial_sign;
		current_oscilating_time = internal_timer;
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
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, -game_object->parent->transform->GetLocalRotation().WorldZ() * initial_sign * push_force);
				break;
			}
			case OSCILATION_DIRECTION::Y:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, float3(0, -initial_sign * push_force, 0));
				break;
			}
			case OSCILATION_DIRECTION::Z:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(0, game_object->parent->transform->GetLocalRotation().WorldX() * initial_sign * push_force);
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
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, -game_object->parent->transform->GetLocalRotation().WorldZ() * initial_sign * push_force);
				break;
			}
			case OSCILATION_DIRECTION::Y:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, float3(0, -initial_sign * push_force, 0));
				break;
			}
			case OSCILATION_DIRECTION::Z:
			{
				col->game_object_attached->GetComponent<PlayerController>()->ReceiveDamage(damage_to_do, game_object->parent->transform->GetLocalRotation().WorldX() * initial_sign * push_force);
				break;
			}
			}
			
			break;
		}
		case TYPE::ANY:
			break;
		}

		col->game_object_attached->GetComponent<PlayerController>()->ChangeCollisionLayer("NoOscillator", 0.5f);

	}
}

void Training_Zone::Init()
{
	rb = GetComponent<ComponentRigidBody>();

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

	current_oscilating_time = Time::GetGameTime();
	internal_timer = Time::GetGameTime();

	has_started = true;
}
