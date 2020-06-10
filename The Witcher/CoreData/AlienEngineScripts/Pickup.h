#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(PickUps, COIN, HEALTH_ORB);


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
	PickUps picky = PickUps::HEALTH_ORB;

private:
	float start_time = 0;
};

ALIEN_FACTORY Pickup* CreatePickup() {
	Pickup* alien = new Pickup();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->duration);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->value);
	SHOW_IN_INSPECTOR_AS_STRING(alien->stat_to_change);
	SHOW_IN_INSPECTOR_AS_ENUM(PickUps, alien->picky);
	return alien;
}
