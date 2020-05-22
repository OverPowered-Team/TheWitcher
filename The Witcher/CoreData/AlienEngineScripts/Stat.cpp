#include "Stat.h"
#include "Effect.h"

Stat::Stat(std::string name, float base_value)
{
    this->name = name;
    this->base_value = base_value;
    this->current_value = base_value;
    this->max_value = base_value;
	this->min_value = base_value; 
}

Stat::~Stat()
{
}

void Stat::ApplyEffect(Effect* effect)
{
    effects.push_back(effect);
    CalculateStat();
}

void Stat::RemoveEffect(Effect* effect)
{
    for (std::vector<Effect*>::iterator it = effects.begin(); it != effects.end(); ++it)
    {
        if (effect == (*it))
        {
            effects.erase(it);
            break;
        }
    }
    CalculateStat();
}

void Stat::CalculateStat()
{
    float additive_value = 0;
    float mult_value = 0;

    for (std::vector<Effect*>::iterator it = effects.begin(); it != effects.end(); ++it)
    {
        additive_value += (*it)->GetAdditiveAmount(name);
        mult_value += (*it)->GetMultiplicativeAmount(name);
    }

    float old_max = max_value;
    max_value = base_value;
    max_value += additive_value;
    if(mult_value > 0)
        max_value *= mult_value;

    if (old_max != 0)
        current_value = (current_value * max_value) / old_max;
    else
        current_value = max_value;
}

void Stat::ModifyCurrentStat(Effect* _effect)
{
    current_value += _effect->GetAdditiveAmount(name);
}

void Stat::SetBaseStat(float _value)
{
	base_value = _value;
	current_value = base_value;
}

void Stat::SetCurrentStat(float value)
{
     
    if (value >= min_value && value <= max_value)
        current_value = value;
	else if (value < min_value)
		current_value = min_value;
    else if (value > max_value)
        current_value = max_value;
}

void Stat::SetMaxValue(float _value)
{
    max_value = _value;
}

void Stat::SetMinValue(float _value)
{
	min_value = _value;
}

void Stat::IncreaseStat(float value)
{
    if (current_value + value < max_value) {
        current_value += value;
    }
    else {
        current_value = max_value;
    }
}

void Stat::DecreaseStat(float value)
{
    if (current_value - value > 0) {
        current_value -= value;
    }
    else {
        current_value = 0;
    }
}

void Stat::FillStats(std::map<std::string, Stat> &stats, JSONArraypack* json)
{
    if (json)
    {
        json->GetFirstNode();
        for (int i = 0; i < json->GetArraySize(); ++i)
        {
            Stat stat = Stat(json->GetString("name"), json->GetNumber("value"));
            stats.insert(std::pair(stat.name, stat));
            json->GetAnotherNode();
        }
    }
}
