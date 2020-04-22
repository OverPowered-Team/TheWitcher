#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ArrowScript : public Alien {

public:

	ArrowScript();
	virtual ~ArrowScript();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);

public:
	float life_time = 5.0F;
	float damage = 0.0F;
	float3 direction;

private:
	float current_time = 0.0F;
};

ALIEN_FACTORY ArrowScript* CreateArrowScript() {
	ArrowScript* arrow = new ArrowScript();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(arrow->life_time);
	return arrow;
} 
