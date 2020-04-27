#pragma once
#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerController;
class AttackEffect;
class CameraShake;

class Attack {
	friend class PlayerAttacks;
public:
	struct AttackInfo {
		std::string name = "";
		std::string input = "";
		std::string particle_name = "";
		float3 collider_position;
		float3 collider_size;
		Stat base_damage;
		float movement_strength = 0.0f;
		float max_snap_distance = 0.0f;
		float freeze_time = 0.0f;
		int activation_frame = 0;
		std::string next_light = "";
		std::string next_heavy = "";
		int shake = 0;
	};
public:

	Attack() {};
	Attack(AttackInfo info)
	{
		this->info = info;
	}
	bool IsLast() { return (light_attack_link == nullptr && heavy_attack_link == nullptr); }
public:
	AttackInfo info;

private:
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
	void StartSpell(uint spell_index); //maybe have enum for spells?
	void UpdateCurrentAttack();
	void ReceiveInput(AttackType attack);
	void CleanUp();

	std::vector<std::string> GetFinalAttacks();
	void OnAddAttackEffect(AttackEffect* new_effect);
	void CancelAttack();
	void ActivateCollider();
	void DeactivateCollider();

	void AllowCombo();
	void OnDrawGizmosSelected();
	bool CanBeInterrupted();

	void OnAnimationEnd(const char* name);

	void AttackShake();
	float GetCurrentDMG();
	Attack* GetCurrentAttack();

public:
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

	GameObject* current_target = nullptr;
	PlayerController* player_controller = nullptr;
	ComponentBoxCollider* collider = nullptr;
	CameraShake* shake = nullptr;
	std::vector<Attack*> attacks;
	std::vector<Attack*> spells;

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

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_detection_range);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->max_snap_angle);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_angle_value);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_distance_value);

	SHOW_VOID_FUNCTION(PlayerAttacks::ActivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DeactivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AllowCombo, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AttackShake, player_attacks);
	return player_attacks;
} 
