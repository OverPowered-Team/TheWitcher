#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ReverbZone : public Alien {

public:

	ReverbZone();
	virtual ~ReverbZone();
	
	void Start();
	void Update();

	void OnTrigger(ComponentCollider* collider);
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);

	std::string zone_name = "normal";
	float strength = 1.f;
};

ALIEN_FACTORY ReverbZone* CreateReverbZone() {
	ReverbZone* alien = new ReverbZone();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_STRING(alien->zone_name);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->strength, 0.f, 1.f);

	return alien;
} 
