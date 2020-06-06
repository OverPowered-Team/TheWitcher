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
	float starting_y_position = 0.0f;
	bool is_last = false;
};

class ALIEN_ENGINE_API UI_DamageCount : public Alien {

public:

	UI_DamageCount();
	virtual ~UI_DamageCount();
	
public:

	void AddDamageCount(float damage, PlayerController* player);

	void PlayerHasBeenHit(PlayerController* player);

public:

	Prefab text;

private:

	void Start();
	void Update();

	void DamageCount_Handling(int index);

	void ScaleDamageCount(int index);

	void Shake(int index);

private:

	// vectors
	std::vector<DamageNum*> transition_player1_damagenums;
	std::vector<DamageNum*> player1_damagenums;
	std::vector<DamageNum*> transition_player2_damagenums;
	std::vector<DamageNum*> player2_damagenums;

	ComponentText* damagecount_player1 = nullptr;
	ComponentText* damagecount_player2 = nullptr;

	float	internal_timer = 0.0f;
	float	time_paused = 0.0f;

	// TimeCount
	float	damage_count1_time = 0.0f;
	float	damage_count2_time = 0.0f;

	// Effects ------------------
	// Fade

	bool	is_fading_in1 = false;
	bool	is_fading_in2 = false;
	float	fadein_timer1 = 0.0f;
	float	fadein_timer2 = 0.0f;

	// Scaling

	bool	is_scaling1 = false;
	bool	is_scaling2 = false;
	float	original_scale = 0.0f;

	float	scaling_time1 = 0.0f;
	float	scaling_time2 = 0.0f;

	// Shaking

	float	original_position_x1 = 0.0f;
	float	original_position_x2 = 0.0f;
	float	original_position_y1 = 0.0f;
	float	original_position_y2 = 0.0f;

	bool	is_shaking1 = false;
	bool	is_shaking2 = false;

	float	off_set = 15.0f;
	float	post_off_set = 0.0f;

	float	start_shake_time1 = 0.0f;
	float	start_shake_time2 = 0.0f;

	// --------------------------
};

ALIEN_FACTORY UI_DamageCount* CreateUI_DamageCount() {
	UI_DamageCount* alien = new UI_DamageCount();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->text);

	return alien;
} 
