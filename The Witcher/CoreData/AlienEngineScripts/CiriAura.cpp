#include "CiriAura.h"

CiriAura::CiriAura() : Enemy()
{
}

CiriAura::~CiriAura()
{
}

void CiriAura::UpdateEnemy()
{
	switch (state)
	{
	case CiriAura::AuraState::NONE:
		break;
	case CiriAura::AuraState::APPEAR:
	{
		Appear();
		break;
	}
	
	case CiriAura::AuraState::IDLE:
		break;
	case CiriAura::AuraState::MOVE:
	{
		Move();
		break;
	}
	case CiriAura::AuraState::DISAPPEAR:
	{
		Move();
		Disappear();
		break;
	}
	default:
		break;
	}
}

void CiriAura::Appear()
{
	auto meshes = game_object->GetChild("Meshes"); 
	for (auto& mesh : meshes->GetChildren())
	{
		auto material = mesh->GetComponent<ComponentMaterial>();
		auto color = material->material->color; 
		color.w += fadeMaterialSpeed * Time::GetDT(); 
		if (color.w >= 1.f)
		{
			color.w = 1.f;
			game_object->GetComponent<ComponentAnimator>()->SetBool("Appeared", true);
			state = AuraState::IDLE; 
			LOG("Ciri Aura has fully appeared!");
		}
		material->material->color = color; 
	}
}

void CiriAura::Disappear()
{
	auto meshes = game_object->GetChild("Meshes");
	for (auto& mesh : meshes->GetChildren())
	{
		auto material = mesh->GetComponent<ComponentMaterial>();
		auto color = material->material->color;
		color.w -= fadeMaterialSpeed * Time::GetDT();
		if (color.w <= 0.f)
		{
			LOG("Ciri Aura has fully DISAPPEARED!");
			GameObject::Destroy(game_object->parent);
		}
		
		material->material->color = color;
	}
}


void CiriAura::Move()
{
	float3 velocity_vec = movement_direction.Normalized() * movement_speed * Time::GetDT();

	current_mov_distance += velocity_vec.Length();

	if (current_mov_distance >= mov_distance)
	{
		if (state != AuraState::DISAPPEAR)
		{
			state = AuraState::DISAPPEAR;
			return;
		}
	}

	LOG("Ciri Aura moves with direction: (%f,%f,%f)", velocity_vec.x, velocity_vec.y, velocity_vec.z);

	auto pos = game_object->transform->GetGlobalPosition();
	transform->SetGlobalPosition(pos + velocity_vec);

	float angle = atan2f(velocity_vec.z, velocity_vec.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

