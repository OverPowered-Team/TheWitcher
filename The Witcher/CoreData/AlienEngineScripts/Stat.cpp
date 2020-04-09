#include "Stat.h"
#include "Effect.h"

Stat::Stat(std::string name, float base_value)
{
    this->name = name;
    this->base_value = base_value;
    this->current_value = base_value;
    this->max_value = base_value;
}

Stat::~Stat()
{
}

void Stat::CalculateStat(std::vector<Effect*> _effects)
{
    float additive_value = 0;
    float mult_value = 0;

    for (std::vector<Effect*>::iterator it = _effects.begin(); it != _effects.end(); ++it)
    {
        additive_value += (*it)->GetAdditiveAmount(name);
        mult_value += (*it)->GetMultiplicativeAmount(name);
    }

    float old_max = max_value;
    max_value = base_value;
    max_value += additive_value;
    max_value += max_value * mult_value;
    current_value = (current_value * max_value) / old_max;
}

void Stat::ModifyCurrentStat(Effect* _effect)
{
    current_value += current_value + _effect->GetAdditiveAmount(name);
    current_value += current_value * _effect->GetMultiplicativeAmount(name);
}

void Stat::SetBaseStat(float _value)
{
	base_value = _value;
	current_value = base_value;
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
