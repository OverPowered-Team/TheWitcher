#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerProjectile : public Alien {

public:

	PlayerProjectile();
	virtual ~PlayerProjectile();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);

public:
	float damage = 0.0f;
	float duration = 0.0f;
	float speed = 0.0f;
	float knock_back = 0.0f;
	float3 direction = float3::zero();

	PlayerController* player;

private:
	ComponentAudioEmitter* audio = nullptr;
	float start_time = 0.0f;
};

ALIEN_FACTORY PlayerProjectile* CreatePlayerProjectile() {
	PlayerProjectile* alien = new PlayerProjectile();

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->damage);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->duration);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->knock_back);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	// To show in inspector here

	return alien;
} 
