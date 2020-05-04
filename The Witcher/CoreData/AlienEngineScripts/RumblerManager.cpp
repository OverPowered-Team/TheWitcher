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
}

void RumblerManager::Update()
{
	for (auto item = rumblers.begin(); item != rumblers.end();) 
	{
		if ((*item)->rumbler_start_time > (*item)->rumbler_end_time || (*item)->increase_decrease_time == 0)
		{
			delete (*item);
			item = rumblers.erase(item);
		}
		else if ((*item)->increase_decrease && (*item)->rumbler_start_time + 0.01f <= Time::GetGameTime())
		{
			UpdateRumbler((*item));
			++item;
		}
		else
			++item;
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
	Rumbler* rumbler = new Rumbler();
	rumbler->type = _type;
	FillRumbler(&rumbler);

	return rumbler;
}

void RumblerManager::FillRumbler(Rumbler** _rumbler)
{
	JSONfilepack* rumbler_json = JSONfilepack::GetJSON("GameData/Rumblers.json");

	JSONArraypack* rumbler_array = rumbler_json->GetArray("RUMBLER");

	if (rumbler_array)
	{
		rumbler_array->GetFirstNode();

		for (uint i = 0; i < rumbler_array->GetArraySize(); i++)
		{
			if (GetType(rumbler_array->GetString("type")) != (*_rumbler)->type)
				rumbler_array->GetAnotherNode();
			else
				break;
		}
		(*_rumbler)->name = rumbler_array->GetString("name");
		(*_rumbler)->strength = rumbler_array->GetNumber("strength");
		(*_rumbler)->duration = rumbler_array->GetNumber("duration");
		rumblers.push_back((*_rumbler));
	}

	JSONfilepack::FreeJSON(rumbler_json);
}

RumblerType RumblerManager::GetType(std::string _name_type)
{
	RumblerType type = RumblerType::NONE;

	if (strcmp(_name_type.data(), "RECEIVE_HIT") == 0)
		type = RumblerType::RECEIVE_HIT;
	else if (strcmp(_name_type.data(), "LIGHT_ATTACK") == 0)
		type = RumblerType::LIGHT_ATTACK;
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

void RumblerManager::CalculateTime(Rumbler* _rumbler)
{
	if (_rumbler->increase_decrease_time > 0)
	{
		if (!_rumbler->increase_decrease)
		{
			_rumbler->increase_decrease = true;
			_rumbler->rumbler_end_time = Time::GetGameTime() + _rumbler->increase_decrease_time;
		}

		_rumbler->rumbler_start_time = Time::GetGameTime();

		if (_rumbler->type == RumblerType::INCREASING)
			_rumbler->strength += 1 / (_rumbler->increase_decrease_time * 100);
		else if (_rumbler->type == RumblerType::DECREASING)
			_rumbler->strength -= 1 / (_rumbler->increase_decrease_time * 100);
		
	}
}

void RumblerManager::StartRumbler(RumblerType _type, int _index_controller, float _increase_decrease_time)
{
	if (_index_controller > 0)
	{
		Rumbler* rumbler = GetRumbler(_type);
		rumbler->increase_decrease_time = _increase_decrease_time;
		rumbler->index_controller = _index_controller;
		if (rumbler != nullptr)
		{
			Input::DoRumble(rumbler->index_controller, rumbler->strength, rumbler->duration);
			CalculateTime(rumbler);
		}
	}
	else
	{
		StartRumbler(_type, 1, _increase_decrease_time);
		StartRumbler(_type, 2, _increase_decrease_time);
	}
}

void RumblerManager::StartRumbler(std::string _name, int _index_controller)
{
	RumblerType _type = GetType(_name);
	StartRumbler(_type, _index_controller);
}

void RumblerManager::UpdateRumbler(Rumbler* _rumbler)
{
	if (_rumbler != nullptr)
	{
		Input::DoRumble(_rumbler->index_controller, _rumbler->strength, _rumbler->duration);
		CalculateTime(_rumbler);
	}
}

