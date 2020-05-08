#include "UI_Ultibar_Particle.h"

UI_Ultibar_Particle::UI_Ultibar_Particle() : Alien()
{
}

UI_Ultibar_Particle::~UI_Ultibar_Particle()
{
}

void UI_Ultibar_Particle::Start()
{
	particle_time = Time::GetGameTime();
	original_scale = transform->GetLocalScale().x;
}

void UI_Ultibar_Particle::Update()
{
	float t = (Time::GetGameTime() - particle_time) / 0.25f;
	float lerp = 0.0f;

	if (sign > 0)
	{
		lerp = Maths::Lerp(original_scale, original_scale * (1.f + 1.f / 3.f), t);
	}
	else
	{
		lerp = Maths::Lerp(original_scale * (1.f + 1.f / 3.f), original_scale, t);
	}

	if (t >= 1)
	{
		sign = -sign;
		particle_time = Time::GetGameTime();
	}
}