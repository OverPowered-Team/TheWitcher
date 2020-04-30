#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Bonfire : public Alien {

public:

	Bonfire();
	virtual ~Bonfire();
	
	void Start();
	void Update();

	void OnDrawGizmos() override;

public:

	float bonfire_radius = 2.0f;
};

ALIEN_FACTORY Bonfire* CreateBonfire() {
	Bonfire* alien = new Bonfire();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->bonfire_radius);

	return alien;
} 
