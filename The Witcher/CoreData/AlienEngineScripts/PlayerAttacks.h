#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class Attack {
public:
	Attack() {};
	Attack(const char* name, const char* input, float3 coll_pos, float3 coll_size, float mult, const char* n_light, const char* n_heavy)
	{
		this->name = name;
		this->input = input;
		this->collider_position = coll_pos;
		this->collider_size = coll_size;
		this->base_damage = mult;
		this->next_light = n_light;
		this->next_heavy = n_heavy;
	}

	std::string name = "";
	std::string input = "";
	float3 collider_position;
	float3 collider_size;
	float base_damage = 0.0f;
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
	void ComboAttack(AttackType new_attack);

	void ActiveCollider();
	void DesactiveCollider();

	void OnAnimationEnd(const char* name);

public:
	float input_window = 0.0f;

protected:
	void CreateAttacks();
	void ConnectAttacks();
	void DoAttack();
	void SelectAttack(AttackType attack);
	bool CanReceiveInput();

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;
	PlayerController* player_controller = nullptr;
	ComponentBoxCollider* collider = nullptr;

	float attack_input_time = 0.0f;

	std::vector<Attack*> attacks;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->input_window);
	SHOW_VOID_FUNCTION(PlayerAttacks::ActiveCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DesactiveCollider, player_attacks);

	return player_attacks;
} 
