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

	void OnDrawGizmosSelected() override;

public:

	void RecieveCollisionEnterInteraction(int collider_index);
	void RecieveCollisionExitInteraction(int collider_index);

	float CalculateRatio();

	ComponentCamera* camera = nullptr;
	 
	float3 centerPoint = float3::zero();
	float3 fogColor = float3::one();

	float targetFogDensity = 0.035;
	float targetFogGradient = 1.35;

	float currentDensity = 0.f;
	float currentGradient = 0.f;

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
	SHOW_SEPARATOR();
	SHOW_TEXT("Fog Properties");
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->targetFogDensity, 0.f, 0.5f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->targetFogGradient, 0.f, 2.f);
	SHOW_TEXT("Fog Color");
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->fogColor.x, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->fogColor.y, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->fogColor.z, 0.f, 1.f);
	SHOW_SEPARATOR();
	SHOW_TEXT("Current State");
	SHOW_IN_INSPECTOR_AS_ENUM(FogState, alien->fogState);


	return alien;
} 
