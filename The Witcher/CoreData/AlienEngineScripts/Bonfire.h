#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Bonfire : public Alien {

public:

	Bonfire();
	virtual ~Bonfire();
	
	void Start();

	void OnTriggerEnter(ComponentCollider* collider) override;

	void ManageTrigger();

public:
	// GameObject
	GameObject* ui_bonfire = nullptr;

};

ALIEN_FACTORY Bonfire* CreateBonfire() {
	Bonfire* alien = new Bonfire();
	// To show in inspector here

	return alien;
} 
