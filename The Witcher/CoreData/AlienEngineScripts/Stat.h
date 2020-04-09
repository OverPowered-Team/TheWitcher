#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Effect;

class Stat {
public:

	Stat() {};
	Stat(std::string name, float base_value);
	~Stat();

	void CalculateStat(std::vector<Effect*> _effects);
	float GetValue() { return current_value; }
	void SetBaseStat(float _value);
	void IncreaseStat(float value);
	void DecreaseStat(float value);

	std::string name;
	float base_value;
	float current_value;
	float max_value;
};