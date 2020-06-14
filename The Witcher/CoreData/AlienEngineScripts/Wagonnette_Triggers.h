#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Wagonnette_Triggers : public Alien {

public:

	enum(TRIGGER_TYPE,
		DUCKING,
		DODGING)

	Wagonnette_Triggers();
	virtual ~Wagonnette_Triggers();
	
	void OnTriggerEnter(ComponentCollider* col) override;

public:

	TRIGGER_TYPE type = TRIGGER_TYPE::DUCKING;

};

ALIEN_FACTORY Wagonnette_Triggers* CreateWagonnette_Triggers() {
	Wagonnette_Triggers* alien = new Wagonnette_Triggers();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(Wagonnette_Triggers::TRIGGER_TYPE, alien->type);

	return alien;
} 
