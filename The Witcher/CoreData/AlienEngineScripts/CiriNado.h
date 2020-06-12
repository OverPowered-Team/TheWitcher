#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CiriNado : public Alien {

public:

	CiriNado();
	virtual ~CiriNado();
	float3 knock_back;
	float knock_force = 8.0f;
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);
};

ALIEN_FACTORY CiriNado* CreateCiriNado() {
	CiriNado* alien = new CiriNado();
	// To show in inspector here

	return alien;
} 
