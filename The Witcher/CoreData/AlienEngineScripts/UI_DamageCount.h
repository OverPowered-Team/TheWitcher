#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class DamageNum
{
public:
	GameObject* go = nullptr;
	ComponentText* text = nullptr;
	int damage = 0.0f;
	float current_timer = 0.0f;
	bool is_transitioning = false;
	float starting_y_position = 0.0f;
};

class ALIEN_ENGINE_API UI_DamageCount : public Alien {

public:

	UI_DamageCount();
	virtual ~UI_DamageCount();
	
public:

	void AddDamageCount(float damage, PlayerController* player);

private:

	void Start();
	void Update();

private:

	// vectors
	std::vector<DamageNum*> player1_damagenums;
	std::vector<DamageNum*> player2_damagenums;

	ComponentText* damagecount_player1 = nullptr;
	ComponentText* damagecount_player2 = nullptr;

	float internal_timer = 0.0f;
	float time_paused = 0.0f;

	// TimeCount
	float damage_count1_time = 0.0f;
	float damage_count2_time = 0.0f;

};

ALIEN_FACTORY UI_DamageCount* CreateUI_DamageCount() {
	UI_DamageCount* alien = new UI_DamageCount();
	// To show in inspector here

	return alien;
} 
