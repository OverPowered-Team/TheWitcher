#include "Stat.h"
#include "Effect.h"

Stat::Stat(std::string _name, float _base_value)
{
	name = _name;
	base_value = _base_value;
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

    current_value = base_value;
    current_value += additive_value;
    current_value += current_value * mult_value;
}

void Stat::SetBaseStat(float _value)
{
	base_value = _value;
	current_value = base_value;
}
