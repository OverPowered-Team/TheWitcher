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
		break;

	case FogState::ON:
		break;

	case FogState::TRANSITION:

		float percentage = CalculateRatio();
		camera->SetFogDensity(targetFogDensity * percentage);
		//camera->SetFogGradient(targetFogGradient * percentage);

		break;
	}
}

void ProgressiveFog::RecieveCollisionEnterInteraction(int collider_index)
{
	switch (collider_index)
	{
	case 1:
		fogState = FogState::TRANSITION;

		camera->SetBackgroundColor(fogColor);
		camera->EnableFog();
		camera->SetFogDensity(0.0001f);
		camera->SetFogGradient(targetFogGradient);

		LOG("State in TRANSITION");
		break;

	case 2:
		fogState = FogState::ON;

		camera->SetFogDensity(targetFogDensity);
		//camera->SetFogGradient(targetFogGradient);

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
		camera->DisableFog();
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
	float totalDistance = outterRadius - innerRadius;

	float3 camPos = camera->game_object_attached->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();
	float3 outterPos = camPos;
	camPos.Normalize();
	float3 innerPos = camPos * innerRadius;

	float3 totalPos = outterPos - innerPos;

	float magnitude = totalPos.Length();

	float percentage = magnitude / totalDistance;

	percentage = 1 - percentage;

	percentage = Clamp(percentage, 0.0001f, 1.0f);

	LOG("Percentage: %f", percentage);
	return percentage;
}