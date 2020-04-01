#pragma once
#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerController;

class Attack {
public:
	struct AttackInfo {
		std::string name = "";
		std::string input = "";
		float3 collider_position;
		float3 collider_size;
		Stat* base_damage = nullptr;
		float movement_strength = 0.0f;
		float max_snap_distance = 0.0f;
		int activation_frame = 0;
		std::string next_light = "";
		std::string next_heavy = "";
	};

	Attack() {};
	Attack(AttackInfo info)
	{
		this->info = info;
	}
	void CleanUp();

	AttackInfo info;
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
	void CleanUp();

	std::vector<std::string> GetFinalAttacks();
	void OnAddAttackEffect(std::string _attack_name);
	void ActivateCollider();
	void DeactivateCollider();

	void AllowCombo();
	void OnDrawGizmos();
	bool CanBeInterrupted();

	void OnAnimationEnd(const char* name);

	float GetCurrentDMG();

public:
	GameObject* collider_go = nullptr;
	float snap_detection_range = 5.0f;
	float max_snap_angle = 0.0f;

	float snap_angle_value = 0.0f;
	float snap_distance_value = 0.0f;

protected:
	void CreateAttacks();
	void ConnectAttacks();
	void DoAttack();
	void AttackMovement();
	void SelectAttack(AttackType attack);
	void SnapToTarget();
	bool FindSnapTarget();
	float3 GetMovementVector();

protected:
	Attack* current_attack = nullptr;
	Attack* base_light_attack = nullptr;
	Attack* base_heavy_attack = nullptr;

	GameObject** enemies = nullptr; //this is temporary
	uint enemies_size = 0; //this is temporary

	GameObject* current_target = nullptr;
	PlayerController* player_controller = nullptr;
	ComponentBoxCollider* collider = nullptr;

	std::vector<Attack*> attacks;
	float finish_attack_time = 0.0f;
	float start_attack_time = 0.0f;
	float snap_time = 0.0f;
	float distance_snapped = 0.0f;

	bool can_execute_input = false;
	AttackType next_attack = AttackType::NONE;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(player_attacks->collider_go);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_detection_range);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->max_snap_angle);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_angle_value);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_distance_value);

	SHOW_VOID_FUNCTION(PlayerAttacks::ActivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DeactivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AllowCombo, player_attacks);

	return player_attacks;
} 
