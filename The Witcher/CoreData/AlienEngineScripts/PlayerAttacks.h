#pragma once
#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class EnemyManager;
class Enemy;

class Attack {
public:
	Attack() {};
	Attack(const char* name, const char* input, float3 coll_pos, float3 coll_size, float mult, int a_frame,
		float m_strgth, const char* n_light, const char* n_heavy)
	{
		this->name = name;
		this->input = input;
		this->collider_position = coll_pos;
		this->collider_size = coll_size;
		this->base_damage = mult;
		this->movement_strength = m_strgth;
		this->activation_frame = a_frame;
		this->next_light = n_light;
		this->next_heavy = n_heavy;
	}

	std::string name = "";
	std::string input = "";
	float3 collider_position;
	float3 collider_size;
	float base_damage = 0.0f;
	float movement_strength = 0.0f;
	int activation_frame = 0;

	std::string next_light = "";
	std::string next_heavy = "";

	Attack* light_attack_link = nullptr;
	Attack* heavy_attack_link = nullptr;
};

class ALIEN_ENGINE_API PlayerAttacks : public Alien {

public:
	enum (AttackType,
		LIGHT,
		HEAVY,
		NONE
	);

public:
	PlayerAttacks();
	virtual ~PlayerAttacks();
	
	void Start();
	void StartAttack(AttackType attack);
	void UpdateCurrentAttack();
	void ReceiveInput(AttackType attack);

	void ActivateCollider();
	void DeactivateCollider();
	void AllowCombo();
	void OnDrawGizmos();
	bool CanBeInterrupted();

	void OnAnimationEnd(const char* name);

public:
	GameObject* collider_go = nullptr;
	float snap_range = 0.0f;
	float max_snap_angle = 0.0f;

protected:
	void CreateAttacks();
	void ConnectAttacks();
	void DoAttack();
	void AttackMovement();
	void SelectAttack(AttackType attack);
	float3 CalculateSnapVelocity();
	bool FindSnapTarget();
	float3 GetMovementVector();
	float3 GetAttackImpulse();

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;

	EnemyManager* enemy_manager = nullptr;
	Enemy* current_target = nullptr;
	PlayerController* player_controller = nullptr;
	ComponentBoxCollider* collider = nullptr;

	std::vector<Attack*> attacks;
	float finish_attack_time = 0.0f;
	float start_attack_time = 0.0f;
	float snap_time = 0.0f;

	bool can_execute_input = false;
	AttackType next_attack = AttackType::NONE;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player_attacks->collider_go);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_range);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->max_snap_angle);

	SHOW_VOID_FUNCTION(PlayerAttacks::ActivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DeactivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AllowCombo, player_attacks);

	return player_attacks;
} 
