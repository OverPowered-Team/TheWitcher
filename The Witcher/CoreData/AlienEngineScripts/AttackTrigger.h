#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API AttackTrigger : public Alien
{
public:
	AttackTrigger();
	virtual ~AttackTrigger();

	void OnTriggerEnter(ComponentCollider* collider);
	GameObject* player_obj = nullptr;

public:
	void Start();

private:
	PlayerController* player = nullptr;
};

ALIEN_FACTORY AttackTrigger* CreateAttackTrigger() {
	AttackTrigger* alien = new AttackTrigger();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->player_obj);

	return alien;
}

