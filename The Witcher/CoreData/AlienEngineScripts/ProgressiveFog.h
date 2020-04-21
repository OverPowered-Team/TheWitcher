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

	GameObject* innerSphere = nullptr; 
	GameObject* outterSphere = nullptr; 
	FogState fogState = FogState::OFF;
};

ALIEN_FACTORY ProgressiveFog* CreateProgressiveFog() {
	ProgressiveFog* alien = new ProgressiveFog();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(FogState, alien->fogState);

	return alien;
} 
