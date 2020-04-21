#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ProgressiveFog; 

class ALIEN_ENGINE_API TriggerFog : public Alien {

public:

	TriggerFog();
	virtual ~TriggerFog();
	
	void OnTriggerEnter(ComponentCollider* collider) override;
	void OnTriggerExit(ComponentCollider* collider) override;

	void Start();
	void Update();

	int collider_index = 0;
	ProgressiveFog* parentScript = nullptr;
};

ALIEN_FACTORY TriggerFog* CreateTriggerFog() {
	TriggerFog* alien = new TriggerFog();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(alien->collider_index, 1, 2);

	return alien;
} 
