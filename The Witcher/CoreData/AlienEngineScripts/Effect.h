#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

struct Modifier
{
	float amount;
	std::string identifier;
};
class Effect {
public:
	Effect();
	virtual ~Effect();

	void AddFlatModifier(float value, std::string identifier);
	void AddMultiplicativeModifier(float value, std::string identifier);
	float GetAdditiveAmount(std::string identifier);
	float GetMultiplicativeAmount(std::string identifier);

	std::string name = "";
	std::vector<Modifier> additive_modifiers;
	std::vector<Modifier> multiplicative_modifiers;

	void (*OnHit)(GameObject enemy);

};

class AttackEffect : public Effect {
public:
	AttackEffect();
	virtual ~AttackEffect();

	void SetAttackIdentifier(std::string identifier);
	std::string GetAttackIdentifier();

	std::string attack_name = "";
};


