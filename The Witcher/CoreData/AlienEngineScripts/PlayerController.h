#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerAttacks;
class Relic;
class Effect;
class Enemy;

class ALIEN_ENGINE_API PlayerController : public Alien {

public:

	enum (PlayerState,
		IDLE,
		RUNNING,
		BASIC_ATTACK,
		JUMPING,
		DASHING,
		CASTING,
		DEAD,
		REVIVING,
		HIT,
		ROOT,

		MAX
		);

	enum (PlayerType,
		GERALT,
		YENNEFER,

		MAX
		);


	struct PlayerData {
		PlayerType type = PlayerType::GERALT;
		std::map<std::string, Stat> stats;

		//BASIC MOVEMENT DATA
		float3 speed = float3::zero();
		float jump_power = 25.f;
		float gravity = 9.8f;

		//RECOUNT
		float total_damage_dealt = 0.0f;
		uint total_kills = 0;
	};

public:
	PlayerController();
	virtual ~PlayerController();

	void Start();
	void Update();
	void PreUpdate();

	void UpdateInput();
	void IdleInput();
	void RunningInput();
	void AttackingInput();
	void ApplyRoot(float time);
	void ReleaseFromRoot();

	bool AnyKeyboardInput();

	void HandleMovement();
	void EffectsUpdate();
	void Jump();
	void Fall();
	void Roll();
	void OnAnimationEnd(const char* name);
	void PlayAttackParticle();
	void Die();
	void Revive();
	void ActionRevive();
	void ReceiveDamage(float value, float3 knock_back = {0, 0, 0});

	//Relics
	void PickUpRelic(Relic* _relic);
	void AddEffect(Effect* _effect);

	bool CheckBoundaries();
	void OnDrawGizmosSelected();
	bool CheckForPossibleRevive();

	void OnUltimateActivation(float value);
	void OnUltimateDeactivation(float value);
	void OnHit(Enemy* enemy, float dmg_dealt);
	void OnEnemyKill();
	void OnTriggerEnter(ComponentCollider* col);

	void HitFreeze(float freeze_time);

	void RemoveFreeze(float speed);

private:
	void LoadStats();
	void InitKeyboardControls();
	void CalculateAABB();

public:
	int controller_index = 1;
	PlayerState state = PlayerState::IDLE;
	PlayerAttacks* attacks = nullptr;
	PlayerData player_data;
	PlayerController* player_being_revived = nullptr;

	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* controller = nullptr;
	float2 movement_input;
	bool mov_input = false;

	float revive_range = 5.0f;

	std::map<std::string, GameObject*> particles;

	//Keyboard input
	SDL_Scancode keyboard_move_up;
	SDL_Scancode keyboard_move_left;
	SDL_Scancode keyboard_move_right;
	SDL_Scancode keyboard_move_down;
	SDL_Scancode keyboard_jump;
	SDL_Scancode keyboard_dash;
	SDL_Scancode keyboard_light_attack;
	SDL_Scancode keyboard_heavy_attack;
	SDL_Scancode keyboard_spell;
	SDL_Scancode keyboard_revive;
	SDL_Scancode keyboard_ultimate;

	//Joystick input
	Input::CONTROLLER_BUTTONS controller_jump = Input::CONTROLLER_BUTTON_A;
	Input::CONTROLLER_BUTTONS controller_dash = Input::CONTROLLER_BUTTON_RIGHTSHOULDER;
	Input::CONTROLLER_BUTTONS controller_light_attack = Input::CONTROLLER_BUTTON_X;
	Input::CONTROLLER_BUTTONS controller_heavy_attack = Input::CONTROLLER_BUTTON_Y;
	Input::CONTROLLER_BUTTONS controller_spell = Input::CONTROLLER_BUTTON_DPAD_UP;
	Input::CONTROLLER_BUTTONS controller_ultimate = Input::CONTROLLER_BUTTON_LEFTSHOULDER;
	Input::CONTROLLER_BUTTONS controller_revive = Input::CONTROLLER_BUTTON_B;

	//Relics
	std::vector<Effect*> effects;
	std::vector<Relic*> relics;

	//UI 
	GameObject* HUD = nullptr;

	float delay_footsteps = 0.5f;

	bool godmode = false;

private:
	float angle = 0.0f;
	float timer = 0.f;
	ComponentAudioEmitter* audio = nullptr;

	ComponentCamera* camera = nullptr;
	AABB max_aabb;
};

ALIEN_FACTORY PlayerController* CreatePlayerController() {
	PlayerController* player = new PlayerController();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(player->controller_index, 1, 2);
	SHOW_IN_INSPECTOR_AS_ENUM(PlayerController::PlayerType, player->player_data.type);
	SHOW_IN_INSPECTOR_AS_ENUM(PlayerController::PlayerState, player->state);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.gravity);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->revive_range);

	SHOW_VOID_FUNCTION(PlayerController::PlayAttackParticle, player);
	SHOW_VOID_FUNCTION(PlayerController::ActionRevive, player);

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->delay_footsteps, 0.01f, 1.f);

	return player;
}

/*class PlayerState {
	PlayerState() {}
	virtual ~PlayerState() {}

	virtual void HandleInput() {}
	virtual void Update() {}
};

class OnGroundState: public PlayerState {
	OnGroundState() {}
	virtual ~OnGroundState() {}

	virtual void HandleInput() {}
	virtual void Update() {}
};*/