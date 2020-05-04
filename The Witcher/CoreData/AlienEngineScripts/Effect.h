#pragma once
class Enemy;
class PlayerController;
class Stat;

struct Modifier
{
	float amount;
	std::string identifier;
};

struct EffectData {
	std::string name = "";
	std::string vfx_on_apply = "";
	std::string vfx_on_tick = "";

	float time = 0.0f;
	float ticks_time = 0.0f;
	uint vfx_position = 0;

	std::vector<Modifier> additive_modifiers;
	std::vector<Modifier> multiplicative_modifiers;
};

class Effect {
public:
	Effect();
	Effect(EffectData* data);
	virtual ~Effect();

	void AddFlatModifier(float value, std::string identifier);
	void AddMultiplicativeModifier(float value, std::string identifier);
	float GetAdditiveAmount(std::string identifier);
	float GetMultiplicativeAmount(std::string identifier);

	bool UpdateEffect();
	bool AffectsStat(std::string stat_name);

	std::string name = "";
	std::vector<Modifier> additive_modifiers;
	std::vector<Modifier> multiplicative_modifiers;

	GameObject* spawned_particle = nullptr;
	uint vfx_position = 0;

	//not sure about this
	std::string vfx_on_apply = "";
	std::string vfx_on_tick = "";

	float time = 0.0f;
	float ticks_time = 0.0f;
	float last_tick_time = 0.0f;
	float start_time = 0.0f;
	bool to_delete = false;

	void (*OnHit)(Enemy* enemy, uint size, EffectData* data);
	void (*OnDash)(Enemy* _enemy, EffectData* data);

};

class AttackEffect : public Effect {
public:
	AttackEffect();
	virtual ~AttackEffect();


	void SetAttackIdentifier(std::string identifier);
	std::string GetAttackIdentifier();

	std::string attack_name = "";
	EffectData* on_hit_effect = nullptr;
};

class DashEffect : public Effect {
public:
	DashEffect();
	virtual ~DashEffect();

	EffectData* on_dash_effect = nullptr;
};



