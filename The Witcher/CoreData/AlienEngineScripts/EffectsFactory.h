#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Effect.h"

#pragma once
class EffectsFactory
{
public:
	EffectsFactory();
	~EffectsFactory();
	Effect* CreateEffect(std::string identifer);

private:
	EffectData* ReadData(JSONArraypack* effect);
	std::map<std::string, EffectData*> effects_data;

};

