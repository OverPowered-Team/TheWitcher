#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API DummyEnemy : public Alien {

public:

	DummyEnemy();
	virtual ~DummyEnemy();
	void OnTriggerEnter(ComponentCollider* col) override;
	//void OnCollisionEnter(const Collision& collision) override;

	Prefab button_x;
	Prefab button_y;
	float maxAngle = 180;

	void Start();
	void Update();

private:
	bool wiggle =  false;
	float perlinNoisePos = 0.0f;
	
	int current_combo_number = 0;
	float time_showing = 0.0f;
	bool showing_combo = false;
	PlayerController* player = nullptr;

	std::vector<GameObject*> current_buttons;

	void DestroyCombo();

	float wiggleDuration = 0;
	float maxWiggleTime = 0.5;

};

ALIEN_FACTORY DummyEnemy* CreateDummyEnemy() {
	DummyEnemy* alien = new DummyEnemy();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->button_x);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->button_y);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->maxAngle);
	return alien;
} 
