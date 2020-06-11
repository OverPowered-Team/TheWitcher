#include "RockOrbit.h"

RockOrbit::RockOrbit() : Alien()
{
}

RockOrbit::~RockOrbit()
{
}

void RockOrbit::Start()
{
}

void RockOrbit::Update()
{
	if (init_timer <= init_time) {
		init_timer += Time::GetDT();
		game_object->transform->AddPosition({ 0.0f, init_velocity, 0.0f });
	}
	game_object->transform->AddRotation(self_rotation);
}