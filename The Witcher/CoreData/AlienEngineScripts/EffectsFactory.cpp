#include "EffectsFactory.h"

EffectsFactory::EffectsFactory()
{
	std::string json_path = "GameData/Effects.json";
	JSONfilepack* json = JSONfilepack::GetJSON(json_path.c_str());

	JSONArraypack* effects = json->GetArray("Effects");
	if (effects)
	{
		effects->GetFirstNode();
		for (int i = 0; i < effects->GetArraySize(); ++i)
		{
			EffectData* data = ReadData(effects);
			effects_data.insert(std::pair(data->name, data));
			effects->GetAnotherNode();
		}
	}
	else
		LOG("Error loading Effects.json");

	JSONfilepack::FreeJSON(json);
}

EffectsFactory::~EffectsFactory()
{
	for (std::map<std::string, EffectData*>::iterator itr = effects_data.begin(); itr != effects_data.end(); itr++)
	{
		delete itr->second;
	}
}

EffectData* EffectsFactory::ReadData(JSONArraypack* effect)
{
	EffectData* data = new EffectData();

	data->name = effect->GetString("name");
	data->time = effect->GetNumber("time");
	data->ticks_time = effect->GetNumber("ticks_time");

	data->vfx_on_apply = effect->GetString("vfx_on_apply");
	data->vfx_on_tick = effect->GetString("vfx_on_tick");
	data->vfx_position = effect->GetNumber("vfx_position");

	JSONArraypack* flat_modifiers = effect->GetArray("flat_modifiers");
	if (flat_modifiers)
	{
		flat_modifiers->GetFirstNode();
		for (uint i = 0; i < flat_modifiers->GetArraySize(); i++)
		{
			Modifier mod;
			mod.identifier = flat_modifiers->GetString("identifier");
			mod.amount = flat_modifiers->GetNumber("value");
			data->additive_modifiers.push_back(mod);
			flat_modifiers->GetAnotherNode();
		}
	}

	JSONArraypack* mult_modifiers = effect->GetArray("multiplicative_modifiers");
	if (mult_modifiers)
	{
		mult_modifiers->GetFirstNode();
		for (uint i = 0; i < mult_modifiers->GetArraySize(); i++)
		{
			Modifier mod;
			mod.identifier = mult_modifiers->GetString("identifier");
			mod.amount = mult_modifiers->GetNumber("value");
			data->multiplicative_modifiers.push_back(mod);
			mult_modifiers->GetAnotherNode();
		}
	}

	return data;
}

Effect* EffectsFactory::CreateEffect(std::string identifier)
{
	if (effects_data[identifier] != nullptr)
	{
		//we may need to do a switch of a type inside effects_data[identifier] to know which type of effect to create
		Effect* new_effect = new Effect(effects_data[identifier]);

		return new_effect;
	}

	return nullptr;
}
