#include "ProgressiveFog.h"

ProgressiveFog::ProgressiveFog() : Alien()
{
}

ProgressiveFog::~ProgressiveFog()
{
}

void ProgressiveFog::Start()
{
}

void ProgressiveFog::Update()
{
}

void ProgressiveFog::RecieveCollisionEnterInteraction(int collider_index)
{
	switch (collider_index)
	{
	case 1:
		fogState = FogState::TRANSITION;
		LOG("State in TRANSITION");
		break;

	case 2:
		fogState = FogState::ON;
		LOG("State ON");
		break;
	}
}

void ProgressiveFog::RecieveCollisionExitInteraction(int collider_index)
{
	switch (collider_index)
	{
	case 1:
		fogState = FogState::OFF;
		LOG("State in OFF");
		break;

	case 2:
		fogState = FogState::TRANSITION;
		LOG("State in TRANSITION");
		break;
	}
}
