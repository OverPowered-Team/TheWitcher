#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API FootTrigger : public Alien {

public:
	enum (MaterialType,
		MUD,
		SAND,
		STONE,
		VEGETATION,
		WOOD
		);

	FootTrigger();
	virtual ~FootTrigger();
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;

public:
	GameObject* player_go = nullptr;

private:
	PlayerController* player = nullptr;
	ComponentRigidBody* rbdy = nullptr;
};

ALIEN_FACTORY FootTrigger* CreateFootTrigger() {
	FootTrigger* alien = new FootTrigger();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->player_go);

	return alien;
}
