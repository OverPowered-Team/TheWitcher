#include "ProgressiveFog.h"

ProgressiveFog::ProgressiveFog() : Alien()
{
}

ProgressiveFog::~ProgressiveFog()
{
}

void ProgressiveFog::Start()
{
	std::vector colliders = game_object->GetComponentsInChildren<ComponentSphereCollider>();

	if (!colliders.empty())
	{
		innerCol = colliders.at(0);
		outterCol = colliders.at(1);
		LOG("Set inner to %f, and outter to %f", innerRadius, outterRadius);
	}
	else
		LOG("Could not get fog colliders");

	camera = game_object->FindWithName("Main Camera")->GetComponent<ComponentCamera>();
}

void ProgressiveFog::Update()
{
	switch (fogState)
	{
	case FogState::OFF:
		camera->DisableFog();

		break;

	case FogState::ON:
		camera->EnableFog();
		camera->SetFogDensity(targetFogDensity);
		camera->SetFogGradient(targetFogGradient);
		break;

	case FogState::TRANSITION:



		break;
	}
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

float ProgressiveFog::CalculateRatio()
{
	
	return 0.0f;
}
