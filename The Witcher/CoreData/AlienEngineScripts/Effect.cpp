#include "Effect.h"

// Effect
Effect::Effect()
{
	
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

    for (std::vector<Modifier>::iterator it = additive_modifiers.begin(); it != additive_modifiers.end(); ++it)
    {
        if ((*it).identifier == identifier)
        {
            final_value += (*it).amount;
        }
    }


    return final_value;
}

float Effect::GetMultiplicativeAmount(std::string identifier)
{
    float final_value = 0.0f;

    for (std::vector<Modifier>::iterator it = multiplicative_modifiers.begin(); it != multiplicative_modifiers.end(); ++it)
    {
        if ((*it).identifier == identifier)
        {
            final_value += (*it).amount;
        }
    }

    return final_value;
}

// AttackEffect
AttackEffect::AttackEffect()
{

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


