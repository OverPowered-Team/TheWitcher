#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Attack {
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
	void Update();

	void ActiveCollider();
	void DesactiveCollider();

protected:
	void CreateAttacks();

protected:
	Attack* current_attack = nullptr;
	std::vector<Attack*> attacks;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_VOID_FUNCTION(PlayerAttacks::ActiveCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DesactiveCollider, player_attacks);

	return player_attacks;
} 
