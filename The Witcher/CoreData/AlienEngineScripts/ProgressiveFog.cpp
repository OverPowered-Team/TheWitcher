#include "ProgressiveFog.h"

ProgressiveFog::ProgressiveFog() : Alien()
{
}

ProgressiveFog::~ProgressiveFog()
{
}

void ProgressiveFog::Start()
{
	LOG("Starting to load Progressive Fog");

	std::vector<ComponentSphereCollider*> colliders = game_object->GetComponentsInChildren<ComponentSphereCollider>();

	if (!colliders.empty())
	{
		outterCol = colliders.at(0);
		innerCol = colliders.at(1);

		if (innerRadius > outterRadius)
		{
			innerRadius = outterRadius;
			LOG("Fog radius are incorrect, Inner Radius must be smaller than Outter Radius");
		}

		innerCol->SetRadius(innerRadius);
		outterCol->SetRadius(outterRadius);
	}
	else
		LOG("Could not get fog colliders");

	GameObject* cam = game_object->FindWithName("Main Camera");

	if (cam != nullptr)
		camera = cam->GetComponent<ComponentCamera>();
	else 
		LOG("Could not find Main camera")

	if(camera == nullptr) 
		LOG("Could not find Main camera")
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

void ProgressiveFog::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(centerPoint, innerRadius * 0.5f, Color::Red());
	Gizmos::DrawWireSphere(centerPoint, outterRadius * 0.5f, Color::Blue());
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

		//LOG("State in TRANSITION");
		break;

	case 2:
		fogState = FogState::ON;

		camera->SetFogDensity(targetFogDensity);
		//camera->SetFogGradient(targetFogGradient);

		//LOG("State ON");
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
		//LOG("State in OFF");
		break;

	case 2:
		fogState = FogState::TRANSITION;
		//LOG("State in TRANSITION");
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