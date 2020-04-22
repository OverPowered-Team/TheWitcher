#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(FogState, 
	OFF, 
	TRANSITION, 
	ON
	);

class ALIEN_ENGINE_API ProgressiveFog : public Alien {

public:

	ProgressiveFog();
	virtual ~ProgressiveFog();
	
	void Start();
	void Update();

public:

	void RecieveCollisionEnterInteraction(int collider_index);
	void RecieveCollisionExitInteraction(int collider_index);

	float CalculateRatio();

	ComponentCamera* camera = nullptr;
	 
	float3 centerPoint = float3::zero();

	float targetFogDensity;
	float targetFogGradient;

	float currentDensity;
	float currentGradient;

	float innerRadius = 20.f; 
	float outterRadius = 25.f;

	ComponentSphereCollider* innerCol = nullptr;
	ComponentSphereCollider* outterCol = nullptr;

	FogState fogState = FogState::OFF;
};

ALIEN_FACTORY ProgressiveFog* CreateProgressiveFog() {
	ProgressiveFog* alien = new ProgressiveFog();
	// To show in inspector here

	SHOW_TEXT("Zone Radius");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->innerRadius);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->outterRadius);
	SHOW_TEXT("Fog Properties");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->targetFogDensity);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->targetFogGradient);
	SHOW_TEXT("Current State");
	SHOW_IN_INSPECTOR_AS_ENUM(FogState, alien->fogState);


	return alien;
} 
