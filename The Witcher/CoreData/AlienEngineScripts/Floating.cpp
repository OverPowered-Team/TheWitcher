#include "Floating.h"

Floating::Floating() : Alien()
{
}

Floating::~Floating()
{
}

void Floating::Start()
{
	if (random)
	{
		time_r = Random::GetRandomIntBetweenTwo(0, 30);
		LOG("Time: %i", time_r);
	}

	time = Time::GetTimeSinceStart();
	sign = 1;
	random_start = false;
}

void Floating::Update()
{
	if (random && !random_start)
	{	
		if (Time::GetTimeSinceStart() > time + float(time_r/10))
		{
			LOG("%i", time_r);
			random_start = true;
		}
	}

	if (!random || random_start)
	{
		game_object->transform->SetGlobalPosition(float3(game_object->transform->GetGlobalPosition().x, game_object->transform->GetGlobalPosition().y + (height * Time::GetDT() / cicle_time) * sign, game_object->transform->GetGlobalPosition().z));
		if (Time::GetTimeSinceStart() > time + cicle_time)
		{
			sign *= -1;
			time = Time::GetTimeSinceStart();
		}
	}
}