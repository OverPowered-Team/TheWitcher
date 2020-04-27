#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"
#include "State.h"

class PlayerAttacks;
class Relic;
class Effect;
class Enemy;

class ALIEN_ENGINE_API PlayerController : public Alien {
	friend class IdleState;
	friend class RunningState;
	friend class JumpingState;
	friend class AttackingState;
	friend class CastingState;
	friend class RevivingState;
	friend class HitState;
public:
	enum (PlayerType,
		GERALT,
		YENNEFER,

		MAX
		);

	struct PlayerData {
		PlayerType type = PlayerType::GERALT;
		std::map<std::string, Stat> stats;

		//OTHER DATA
		float revive_range = 5.0f;

		//BASIC MOVEMENT DATA
		float3 speed = float3::zero();
		float gravity = 9.8f;
		float slow_speed = -0.07f;

		//RECOUNT
		float total_damage_dealt = 0.0f;
		uint total_kills = 0;
	};

public:
	PlayerController();
	virtual ~PlayerController();

	void Start();
	void Update();

	void UpdateInput();
	void SetState(StateType new_state);
	void SwapState(State* new_state);
	void ApplyRoot(float time);
	void ReleaseRoot();

	bool AnyKeyboardInput();

	void HandleMovement();
	void EffectsUpdate();
	void Jump();
	void Fall();
	void OnAnimationEnd(const char* name);
	void PlayAttackParticle();
	void Die();
	void Revive();
	void ActionRevive();
	void ReceiveDamage(float dmg, float3 knock_speed = { 0,0,0 });

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
	State* state;

	PlayerAttacks* attacks = nullptr;
	PlayerData player_data;
	std::map<std::string, GameObject*> particles;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* controller = nullptr;

	float2 movement_input;
	bool mov_input = false;
	bool is_immune = false;
	bool is_rooted = false;

	//Relics
	std::vector<Effect*> effects;
	std::vector<Relic*> relics;

	//UI 
	GameObject* HUD = nullptr;

	//Others
	float delay_footsteps = 0.5f;
	PlayerController* player_being_revived = nullptr;
	bool godmode = false;

	//Input Variables
	int controller_index = 1;
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
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.gravity);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.slow_speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.revive_range);

	SHOW_VOID_FUNCTION(PlayerController::PlayAttackParticle, player);
	SHOW_VOID_FUNCTION(PlayerController::ActionRevive, player);

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->delay_footsteps, 0.01f, 1.f);

	return player;
}