#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"

class ALIEN_ENGINE_API CiriAuraTrigger : public Alien {

public:
	CiriAuraTrigger();
	virtual ~CiriAuraTrigger();
	void Start();
	void OnTriggerEnter(ComponentCollider* collider);


public:
	int trigger_index = 0;
	GameObject* Ciri_Aura = nullptr;

	friend class CiriAuraTrigger;
};

ALIEN_FACTORY CiriAuraTrigger* CreateCiriAuraTrigger() {
	CiriAuraTrigger* alien = new CiriAuraTrigger();
	// To show in inspector here

	SHOW_TEXT("0 = appear, 1 = move, 2 = disappear");
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->trigger_index);


	return alien;
};



