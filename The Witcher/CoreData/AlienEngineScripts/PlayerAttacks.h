#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerController;

class Attack {
public:
	Attack() {};
	Attack(const char* name, const char* input, float3 coll_pos, float3 coll_size, float mult, float m_strgth, const char* n_light, const char* n_heavy)
	{
		this->name = name;
		this->input = input;
		this->collider_position = coll_pos;
		this->collider_size = coll_size;
		this->base_damage = new Stat("Attack_Damage",mult);
		this->movement_strength = m_strgth;
		this->next_light = n_light;
		this->next_heavy = n_heavy;
	}

	std::string name = "";
	std::string input = "";
	float3 collider_position;
	float3 collider_size;
	Stat* base_damage = nullptr;
	float movement_strength = 0.0f;

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
	void ComboAttack();
	void ReceiveInput(AttackType attack);

	std::vector<std::string> GetFinalAttacks();
	void OnAddAttackEffect(std::string _attack_name);
	void ActivateCollider();
	void DeactivateCollider();

	void AllowCombo();
	bool CanBeInterrupted();
	float3 GetAttackImpulse();

	void OnAnimationEnd(const char* name);

public:
	GameObject* collider_go = nullptr;

protected:
	void CreateAttacks();
	void ConnectAttacks();
	void DoAttack();
	void SelectAttack(AttackType attack);

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;

	PlayerController* player_controller = nullptr;
	ComponentBoxCollider* collider = nullptr;

	std::vector<Attack*> attacks;
	float finish_attack_time = 0.0f;

	bool can_execute_input = false;
	AttackType next_attack = AttackType::NONE;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player_attacks->collider_go);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->input_window);
	SHOW_VOID_FUNCTION(PlayerAttacks::ActivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DeactivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AllowCombo, player_attacks);

	return player_attacks;
} 
