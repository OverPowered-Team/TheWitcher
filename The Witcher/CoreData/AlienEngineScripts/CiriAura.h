#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"

class CiriAuraTrigger;
class ALIEN_ENGINE_API CiriAura : public Alien {

public: 
	enum(AuraState,
		NONE = -1,
		APPEAR,
		IDLE,
		MOVE,
		DISAPPEAR);

public:
	CiriAura();
	virtual ~CiriAura();
	void Update();
private:
	void Appear();
	void Disappear();
	void Move();

private:
	AuraState state = AuraState::NONE;
	float current_mov_distance = 0.f;
	bool im_dead_fuck = false;

public:
	float fadeMaterialSpeed = 0.5f; 
	float movement_speed = 5.f;
	float mov_distance = 2.f;
	float3 movement_direction = float3();

	friend class CiriAuraTrigger; 
};

ALIEN_FACTORY CiriAura* CreateCiriAura() {
	CiriAura* alien = new CiriAura();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->fadeMaterialSpeed, 0.2, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->movement_speed, 100.f, 1000.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->movement_direction.x, -1.f, 1.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->movement_direction.y, -1.f, 1.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->movement_direction.z, -1.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->mov_distance, 5.f, 20.f);

	return alien;
};



