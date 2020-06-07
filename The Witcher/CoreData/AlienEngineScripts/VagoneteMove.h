#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "PlayerController.h"

class Wagonnete_UI;

class VagoneteInputs {
public:
	enum class State {
		IDLE,
		INCLINATION,
		COVER
	};

public:

	VagoneteInputs(PlayerController::PlayerType type);

	void Update();

private:

	void UpdateInputs();
	void DoAction();
	void Inclination();

public:

	State state = State::IDLE;

	float currentYInclination = 0.0F;
	float currentInclination = 0.0F;
	int inclinationZone = 1;

	struct {
		SDL_Scancode inclinationLeft = SDL_SCANCODE_UNKNOWN;
		SDL_Scancode inclinationRight = SDL_SCANCODE_UNKNOWN;
		SDL_Scancode cover = SDL_SCANCODE_UNKNOWN;
	}keyboardInput;

	int controllerIndex = -1;

	static Quat playerRotation;
	static float globalInclination;
	static float globalInclinationY;
	static float inclination4player;
	static float speedInclination;
	static State globalState;

	GameObject* player = nullptr;
	VagoneteInputs* other_player = nullptr;
};

class ALIEN_ENGINE_API VagoneteMove : public Alien {

public:

	VagoneteMove();
	virtual ~VagoneteMove();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col);

	void DecreaseLife();

	void SetVelocity(float max_velocity, float acceleration);

public:
	float actual_pos = 0.0F;
	float vagonete_life = 100.f;

	Wagonnete_UI* HUD = nullptr;

	ComponentCurve* curve = nullptr;
	ComponentCurve* next_curve = nullptr;
	ComponentRigidBody* rigid_body = nullptr;

	float current_speed = 0.0F;
	float max_velocity = 0.0F;
	float acceleration = 0.0F;

	bool godmode = false;

private:

	void FollowCurve();

private:

	std::vector<VagoneteInputs*> players;
	float max_life = 0.0f;
};

ALIEN_FACTORY VagoneteMove* CreateVagoneteMove() {
	VagoneteMove* alien = new VagoneteMove();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->current_speed);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->vagonete_life);

	ComponentScript::InspectorDragableFloat(&VagoneteInputs::speedInclination, "speedInclination");
	ComponentScript::InspectorDragableFloat(&VagoneteInputs::inclination4player, "inclination4player");

	return alien;
} 
