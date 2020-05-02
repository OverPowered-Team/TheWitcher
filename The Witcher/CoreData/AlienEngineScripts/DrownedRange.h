#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Drowned.h"

class ALIEN_ENGINE_API DrownedRange : public Drowned {

public:

	DrownedRange();
	virtual ~DrownedRange();
	
	void UpdateEnemy() override;

	void OnAnimationEnd(const char* name) override;
	void OnTriggerEnter(ComponentCollider* collider);

public:
	float current_hide_time = 0.0f;
	float max_hide_time = 3.0f;
};

ALIEN_FACTORY DrownedRange* CreateDrownedRange() {
	DrownedRange* drowned = new DrownedRange();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(drowned->max_hide_time);
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedType, drowned->drowned_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedState, drowned->state);

	return drowned;
} 
