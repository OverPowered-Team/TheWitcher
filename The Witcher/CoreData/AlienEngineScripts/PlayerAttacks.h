#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

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
	void Update();
	void StartAttack();

protected:
	void CreateAttacks();

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;

	std::vector<Attack*> attacks;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* alien = new PlayerAttacks();
	// To show in inspector here

	return alien;
} 
