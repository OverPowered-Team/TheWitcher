#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ExplodeChildren : public Alien {

public:

	ExplodeChildren();
	virtual ~ExplodeChildren();

	void Start() override;

	void Explode();

	void SetVars(float force, float time_despawn);

	void Fall();
	void DeleteMyself();

	std::map<std::string, ComponentParticleSystem*> particles;

private:
	float time_to_disappear = 5.f;
	float force = 15.f;

};

ALIEN_FACTORY ExplodeChildren* CreateExplodeChildren() {
	ExplodeChildren* alien = new ExplodeChildren();
	// To show in inspector here

	return alien;
} 
