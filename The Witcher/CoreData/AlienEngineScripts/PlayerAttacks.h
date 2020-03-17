#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class Attack {
public:
	std::string name = "";
	std::string input = "";
	float3 collider_position;
	float3 collider_size;
	float base_damage = 0.0f;

	Attack* light_attack_link = nullptr;
	Attack* heavy_attack_link = nullptr;
};

class ALIEN_ENGINE_API PlayerAttacks : public Alien {

public:
	PlayerAttacks();
	virtual ~PlayerAttacks();
	
	void Start();
	void StartAttack();
	void UpdateAttack();
	void SaveInput(char input);

	void ActiveCollider();
	void DesactiveCollider();

protected:
	void CreateAttacks();
	void DoAttack();
	void SelectAttack();
	bool CanReceiveInput();

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;
	PlayerController* player_controller = nullptr;

	float start_attack_time = 0.0f;
	float input_window = 0.0f;
	float attack_input_time = 0.0f;

	char last_input = ' ';

	std::vector<Attack*> attacks;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_VOID_FUNCTION(PlayerAttacks::ActiveCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DesactiveCollider, player_attacks);

	return player_attacks;
} 
