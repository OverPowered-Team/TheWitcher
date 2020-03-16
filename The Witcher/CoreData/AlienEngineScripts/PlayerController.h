#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PlayerController : public Alien {

public:

	enum (PlayerState,
		IDLE,
		RUNNING,
		BASIC_ATTACK,
		JUMPING,
		DASHING,

		MAX
	);


	struct PlayerData {
		float movementSpeed = 200.0F;
		float rotationSpeed = 120.0F;
		float currentSpeed = 0.f;
		// dmg, deff, lvl bla bla
	};

public:
	PlayerController();
	virtual ~PlayerController();

	void Start();
	void Update();

	void HandleMovement();

	void OnAnimationEnd(const char* name);

public:
	int controllerIndex = 1;
	PlayerState state = PlayerState::IDLE;
	PlayerData playerData;
	ComponentAnimator* animator = nullptr;
	ComponentRigidBody* rbody = nullptr;


private:
	float angle = 0.0f;
	float stick_threshold = 0.1f;
};

ALIEN_FACTORY PlayerController* CreatePlayerController() {
	PlayerController* player = new PlayerController();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(player->controllerIndex, 1, 2);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->playerData.movementSpeed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->playerData.rotationSpeed);
	SHOW_IN_INSPECTOR_AS_ENUM(PlayerController::PlayerState, player->state);
	return player;
}