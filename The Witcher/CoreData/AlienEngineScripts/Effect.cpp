#include "Effect.h"

// AttackEffect
AttackEffect::AttackEffect()
{
	// Inicialize effects with a json
}

AttackEffect::~AttackEffect()
{
}

void AttackEffect::SetAttackIdentifier(std::string identifier)
{
	attack_name = identifier;
}

std::string AttackEffect::GetAttackIdentifier()
{
	return attack_name;
}

// Effect
Effect::Effect()
{
	// Inicialize effects with a json
}

Effect::~Effect()
{
}

void Effect::AddFlatModifier(float value, std::string identifier)
{
    Modifier new_mod;
    new_mod.amount = value;
    new_mod.identifier = identifier;

    additive_modifiers.push_back(new_mod);
}

void Effect::AddMultiplicativeModifier(float value, std::string identifier)
{
    Modifier new_mod;
    new_mod.amount = value;
    new_mod.identifier = identifier;

    multiplicative_modifiers.push_back(new_mod);
}

float Effect::GetAdditiveAmount(std::string identifier)
{
    float final_value = 0.0f;

    /*foreach(Modifier mod in additive_modifiers)
    {
        if (mod.identifier == identifier)
        {
            final_value += mod.amount;
        }
    }*/

    return final_value;
}

float Effect::GetMultiplicativeAmount(std::string identifier)
{
    float final_value = 0.0f;

    /*foreach(Modifier mod in multiplicative_modifers)
    {
        if (mod.identifier == identifier)
        {
            final_value += mod.amount;
        }
    }*/

    return final_value;
}
