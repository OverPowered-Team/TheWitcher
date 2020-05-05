#pragma once
#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerController;
class AttackEffect;
class CameraShake;
class Enemy;

enum class Attack_Tags {
	T_Spell,
	T_Projectile,
	T_AOE,
	T_Trap,
	T_Buff,
	T_Debuff,
	T_Fire,
	T_Ice,
	T_Earth,
	T_Lightning,
	T_Chaining,
	T_None
};

class Attack {
	friend class PlayerAttacks;
public:
	struct AttackInfo {
		std::string name = "";
		std::string input = "";
		std::string particle_name = "";
		std::string effect = "";
		std::string allow_combo_p_name = "";
		std::string prefab_to_spawn = "";

		std::map<std::string, Stat> stats;
		std::vector<Attack_Tags> tags;

		std::string next_light = "";
		std::string next_heavy = "";

		float3 collider_position;
		float3 collider_size;
		float3 particle_pos;
		float freeze_time = 0.0f;
		float movement_strength = 0.0f;
		float max_distance_traveled = 0.0f;
		float min_distance_to_target = 0.0f;
		float snap_detection_range = 0.0f;
		int shake = 0;
		int activation_frame = 0;
	};

public:

	Attack() {};
	Attack(AttackInfo info)
	{
		this->info = info;
	}
	bool IsLast() { return (light_attack_link == nullptr && heavy_attack_link == nullptr); }
	bool HasTag(Attack_Tags tag) {
		for (int i = 0; i < info.tags.size(); ++i)
		{
			if (info.tags[i] == tag)
				return true;
		}
		return false;
	}
	bool CanHit(Enemy* enemy)
	{
		for (auto it = enemies_hit.begin(); it != enemies_hit.end(); ++it)
		{
			if (enemy == (*it))
				return false;
		}
		return true;
	}
public:
	AttackInfo info;
	std::vector<Enemy*> enemies_hit;

private:
	Attack* light_attack_link = nullptr;
	Attack* heavy_attack_link = nullptr;
};

class ALIEN_ENGINE_API PlayerAttacks : public Alien {

public:
	enum (AttackType,
		LIGHT,
		HEAVY,
		SPELL,
		NONE
	);

public:
	PlayerAttacks();
	virtual ~PlayerAttacks();
	
	void Start();
	void StartAttack(AttackType attack);
	bool StartSpell(uint spell_index); //maybe have enum for spells instead of index?
	void UpdateCurrentAttack();
	void ReceiveInput(AttackType attack, int spell_index = 0);
	void CleanUp();

	std::vector<std::string> GetFinalAttacks();
	void OnAddAttackEffect(AttackEffect* new_effect);
	void OnRemoveAttackEffect(AttackEffect* new_effect);
	void CancelAttack();
	void ActivateCollider();
	void DeactivateCollider();
	void CastSpell();

	void OnHit(Enemy* enemy);

	void AllowCombo();
	bool CanBeInterrupted();

	void AttackShake();
	float GetCurrentDMG();
	Attack* GetCurrentAttack();

public:
	float max_snap_angle = 0.0f;
	float snap_angle_value = 0.0f;
	float snap_distance_value = 0.0f;

protected:
	void CreateAttacks();
	void ConnectAttacks();
	void SpawnChainParticle(float3 from, float3 to);
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
	int next_spell = 0;
};

ALIEN_FACTORY PlayerAttacks* CreatePlayerAttacks() {
	PlayerAttacks* player_attacks = new PlayerAttacks();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->max_snap_angle);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_angle_value);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(player_attacks->snap_distance_value);

	SHOW_VOID_FUNCTION(PlayerAttacks::ActivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::DeactivateCollider, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::CastSpell, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AllowCombo, player_attacks);
	SHOW_VOID_FUNCTION(PlayerAttacks::AttackShake, player_attacks);
	return player_attacks;
} 
