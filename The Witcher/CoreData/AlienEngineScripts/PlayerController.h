#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Relic;
class Effect;

class ALIEN_ENGINE_API PlayerController : public Alien {

public:

	enum (PlayerState,
		IDLE,
		RUNNING,
		BASIC_ATTACK,
		JUMPING,
		DASHING,
		CASTING,

		MAX
		);

	struct PlayerData {
		float movementSpeed = 200.0F;
		float rotationSpeed = 120.0F;
		float currentSpeed = 0.f;
		float dash_power = 1.5f;
		float jump_power = 25.f;
		// dmg, deff, lvl bla bla
	};

public:
	PlayerController();
	virtual ~PlayerController();

	void Start();
	void Update();

	bool AnyKeyboardInput();

	void OnDrawGizmos() override;

	void HandleMovement(float2 joystickInput);
	void OnAttackEffect();
	void OnAnimationEnd(const char* name);
	void PlaySpell();
	void PickUpRelic(Relic* _relic);
	void AddEffect(Effect* _effect);

	bool CheckBoundaries(const float2& joystickInput);

public:
	int controller_index = 1;
	PlayerState state = PlayerState::IDLE;
	PlayerData player_data;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* controller = nullptr;
	bool can_move = false;
	float stick_threshold = 0.1f;
	
	bool keyboard_input = false;

	//Keyboard input
	SDL_Scancode keyboard_move_up;
	SDL_Scancode keyboard_move_left;
	SDL_Scancode keyboard_move_right;
	SDL_Scancode keyboard_move_down;
	SDL_Scancode keyboard_jump;
	SDL_Scancode keyboard_dash;
	SDL_Scancode keyboard_light_attack;
	SDL_Scancode keyboard_spell;

	//Joystick input
	Input::CONTROLLER_BUTTONS controller_jump = Input::CONTROLLER_BUTTON_A;
	Input::CONTROLLER_BUTTONS controller_dash = Input::CONTROLLER_BUTTON_RIGHTSHOULDER;
	Input::CONTROLLER_BUTTONS controller_attack = Input::CONTROLLER_BUTTON_X;
	Input::CONTROLLER_BUTTONS controller_spell = Input::CONTROLLER_BUTTON_B;

	//Relics
	std::vector<Relic*> relics;

	//Particles
	
	GameObject* p_run = nullptr;
	ComponentParticleSystem* c_run = nullptr;

	GameObject* p_attack = nullptr;
	ComponentParticleSystem* c_attack = nullptr;

	GameObject* p_spell = nullptr;
	ComponentParticleSystem* c_spell = nullptr;

	float delay_footsteps = 0.5f;

	GameObject* cube_test = nullptr;

private:
	float angle = 0.0f;
	float timer = 0.f;
	ComponentAudioEmitter* audio = nullptr;

	Frustum* frustum = nullptr;
};

ALIEN_FACTORY PlayerController* CreatePlayerController() {
	PlayerController* player = new PlayerController();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(player->controller_index, 1, 2);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.movementSpeed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.rotationSpeed);
	SHOW_IN_INSPECTOR_AS_ENUM(PlayerController::PlayerState, player->state);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->stick_threshold);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.dash_power);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.jump_power);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player->p_run);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player->p_attack);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player->p_spell);
	SHOW_VOID_FUNCTION(PlayerController::OnAttackEffect, player);
	SHOW_VOID_FUNCTION(PlayerController::PlaySpell, player);

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->delay_footsteps, 0.01f, 1.f);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player->cube_test);

	return player;
}