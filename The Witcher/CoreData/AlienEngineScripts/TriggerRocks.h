#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TriggerRocks : public Alien {

public:

	TriggerRocks();
	virtual ~TriggerRocks();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;

};

ALIEN_FACTORY TriggerRocks* CreateTriggerRocks() {
	TriggerRocks* alien = new TriggerRocks();
	// To show in inspector here

	return alien;
} 
