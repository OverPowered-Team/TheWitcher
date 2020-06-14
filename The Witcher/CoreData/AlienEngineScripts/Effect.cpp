#include "Stat.h"
#include "Effect.h"

// Effect
Effect::Effect()
{
	
}

Effect::Effect(EffectData* data)
{
    name = data->name;
    sound_name = data->sound_name;
    time = data->time;
    ticks_time = data->ticks_time;

    last_tick_time = Time::GetGameTime();
    start_time = Time::GetGameTime();

    vfx_on_apply = data->vfx_on_apply;
    vfx_on_tick = data->vfx_on_tick;
    vfx_position = data->vfx_position;

    for (int i = 0; i < data->additive_modifiers.size(); ++i)
    {
        AddFlatModifier(data->additive_modifiers[i].amount, data->additive_modifiers[i].identifier);
    }
    for (int i = 0; i < data->multiplicative_modifiers.size(); ++i)
    {
        AddMultiplicativeModifier(data->multiplicative_modifiers[i].amount, data->multiplicative_modifiers[i].identifier);
    }
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

bool Effect::UpdateEffect()
{
    if (time > 0)
    {
        if (Time::GetGameTime() > start_time + time)
        {
            to_delete = true;
        }
        else if (ticks_time > 0 && ticks_time + last_tick_time < Time::GetGameTime())
        {
            last_tick_time = Time::GetGameTime();
            return true;
        }
    }
    
    return false;
}

bool Effect::AffectsStat(std::string stat_name)
{
    for (auto it = additive_modifiers.begin(); it != additive_modifiers.end(); ++it)
    {
        if ((*it).identifier == stat_name)
            return true;
    }

    for (auto it = multiplicative_modifiers.begin(); it != multiplicative_modifiers.end(); ++it)
    {
        if ((*it).identifier == stat_name)
            return true;
    }

    return false;
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

// DashEffect
DashEffect::DashEffect()
{
}

DashEffect::~DashEffect()
{
}
