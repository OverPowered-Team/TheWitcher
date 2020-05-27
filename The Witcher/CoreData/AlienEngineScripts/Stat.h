#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Effect;

class Stat {
public:

	Stat() {};
	Stat(std::string name, float base_value);
	~Stat();

	void ApplyEffect(Effect* effect);
	void RemoveEffect(Effect* effect);

	void CalculateStat();
	void ModifyCurrentStat(Effect* _effect);
	float GetValue() { return current_value; }
	float GetMaxValue() { return max_value; }
	float GetBaseValue() { return base_value; }
	void SetBaseStat(float _value);
	void SetCurrentStat(float value);
	void SetMaxValue(float _value);
	void SetMinValue(float _value);
	void IncreaseStat(float value);
	void DecreaseStat(float value);

	static void FillStats(std::map<std::string, Stat> &stats, JSONArraypack* json);

	std::string name;
	std::vector<Effect*> effects;

private:
	
	float base_value = 0.0f;
	float current_value = 0.0f;
	float max_value = 0.0f;
	float min_value = 0.0f; 
};