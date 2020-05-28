#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Trigger_To_Lvl1_Tutorial : public Alien {

public:

	Trigger_To_Lvl1_Tutorial();
	virtual ~Trigger_To_Lvl1_Tutorial();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col) override;
};

ALIEN_FACTORY Trigger_To_Lvl1_Tutorial* CreateTrigger_To_Lvl1_Tutorial() {
	Trigger_To_Lvl1_Tutorial* alien = new Trigger_To_Lvl1_Tutorial();
	// To show in inspector here

	return alien;
} 
