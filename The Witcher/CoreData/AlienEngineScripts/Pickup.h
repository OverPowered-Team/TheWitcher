#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Pickup : public Alien {

public:
	Pickup();
	virtual ~Pickup();

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col);

public:
	float duration = 0;
	float value = 0;
	std::string stat_to_change = "";
private:
	float start_time = 0;
};

ALIEN_FACTORY Pickup* CreatePickup() {
	Pickup* alien = new Pickup();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->duration);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->value);
	SHOW_IN_INSPECTOR_AS_STRING(alien->stat_to_change);

	return alien;
}
