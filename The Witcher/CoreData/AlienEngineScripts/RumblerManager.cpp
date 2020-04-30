#include "RumblerManager.h"


Rumbler::Rumbler()
{
}

Rumbler::~Rumbler()
{
}

RumblerManager::RumblerManager() : Alien()
{
}

RumblerManager::~RumblerManager()
{
	CleanUp();
}

void RumblerManager::Start()
{
	FillRumblers("GameData/Rumblers.json");
}

void RumblerManager::Update()
{
	if (increase_decrease && rumbler_start_time + 0.1f < Time::GetGameTime())
	{
		if(GetRumbler(RumblerType::INCREASING)->strength > 0)
			StartRumbler(RumblerType::INCREASING, index_controller, rumbler_end_time - rumbler_start_time);
		else if (GetRumbler(RumblerType::DECREASING)->strength < 1)
			StartRumbler(RumblerType::DECREASING, index_controller, rumbler_end_time - rumbler_start_time);
	}
}

void RumblerManager::CleanUp()
{
	for (auto item = rumblers.begin(); item != rumblers.end(); ++item) {
		delete (*item);
	}
	rumblers.clear();
}

Rumbler* RumblerManager::GetRumbler(RumblerType _type)
{
	Rumbler* rumbler = nullptr;

	for (auto it = rumblers.begin(); it != rumblers.end(); ++it)
	{
		if((*it)->type == _type)
			rumbler = (*it);
	}
	return rumbler;
}

void RumblerManager::FillRumblers(const char* json_path)
{
	JSONfilepack* rumbler_json = JSONfilepack::GetJSON(json_path);

	JSONArraypack* rumbler_array = rumbler_json->GetArray("RUMBLER");

	if (rumbler_array)
	{
		rumbler_array->GetFirstNode();

		for (uint i = 0; i < rumbler_array->GetArraySize(); i++)
		{
			Rumbler* rumbler = new Rumbler();
			rumbler->type = GetType(rumbler_array->GetString("type"));
			rumbler->name = rumbler_array->GetString("name");
			rumbler->strength = rumbler_array->GetNumber("strength");
			rumbler->duration = rumbler_array->GetNumber("duration");
			rumblers.push_back(rumbler);
			rumbler_array->GetAnotherNode();
		}
	}

	JSONfilepack::FreeJSON(rumbler_json);
}

RumblerType RumblerManager::GetType(std::string _name_type)
{
	RumblerType type = RumblerType::NONE;

	if (strcmp(_name_type.data(), "RECEIVE_HIT") == 0)
		type = RumblerType::RECEIVE_HIT;
	else if (strcmp(_name_type.data(), "HEAVY_ATTACK") == 0)
		type = RumblerType::HEAVY_ATTACK;
	else if (strcmp(_name_type.data(), "LAST_ATTACK") == 0)
		type = RumblerType::LAST_ATTACK;
	else if (strcmp(_name_type.data(), "REVIVE") == 0)
		type = RumblerType::REVIVE;
	else if (strcmp(_name_type.data(), "INCREASING") == 0)
		type = RumblerType::INCREASING;
	else if (strcmp(_name_type.data(), "DECREASING") == 0)
		type = RumblerType::DECREASING;
	
	return type;
}

void RumblerManager::CalculateTime(Rumbler* _rumbler, float _increase_decrease_time)
{
	if (rumbler_start_time > rumbler_end_time)
	{
		increase_decrease = false;

		rumbler_end_time = 0.0f;
		rumbler_start_time = 0.0f;

		if (_rumbler->type == RumblerType::INCREASING)
			_rumbler->strength = 0.0f;
		else if (_rumbler->type == RumblerType::DECREASING)
			_rumbler->strength = 1.0f;
	}
	else if (_increase_decrease_time > 0)
	{
		if (!increase_decrease)
		{
			increase_decrease = true;
			rumbler_end_time = Time::GetGameTime() + _increase_decrease_time;
		}

		rumbler_start_time = Time::GetGameTime();

		if (_rumbler->type == RumblerType::INCREASING)
			_rumbler->strength += 1 / (_increase_decrease_time * 100);
		else if (_rumbler->type == RumblerType::DECREASING)
			_rumbler->strength -= 1 / (_increase_decrease_time * 100);
		
	}
}

void RumblerManager::StartRumbler(RumblerType _type, int _index_controller, float _increase_decrease_time)
{
	
	Rumbler* rumbler = GetRumbler(_type);

	if (rumbler != nullptr)
	{
		if (_index_controller > 0)
		{
			CalculateTime(rumbler, _increase_decrease_time);
			Input::DoRumble(_index_controller, rumbler->strength, rumbler->duration);
		}
		else
		{
			StartRumbler(_type, 1, _increase_decrease_time);
			StartRumbler(_type, 2, _increase_decrease_time);
		}
	}
	
}

void RumblerManager::StartRumbler(std::string _name, int _index_controller)
{
	RumblerType _type = GetType(_name);
	StartRumbler(_type, _index_controller);
}

