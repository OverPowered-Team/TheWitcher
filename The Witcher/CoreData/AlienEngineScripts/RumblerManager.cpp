#include "RumblerManager.h"

RumblerManager::RumblerManager() : Alien()
{
}

RumblerManager::~RumblerManager()
{
}

void RumblerManager::Start()
{
}

void RumblerManager::Update()
{
	if (rumbler_start_time + 0.1f < Time::GetGameTime() && rumbler_start_time < rumbler_end_time)
	{
		rumbler_start_time = Time::GetGameTime();
		if (increasing > 0.0f && increasing < 1.0f)
			StartRumbler(RumblerType::INCREASING, index_controller, rumbler_end_time - rumbler_start_time);
		else
			increasing = 0.0f;


		if (decreasing < 1.0f && decreasing > 0.0f)
			StartRumbler(RumblerType::DECREASING, index_controller, rumbler_end_time - rumbler_start_time);
		else
			decreasing = 1.0f;
	}
	else if (rumbler_start_time >= rumbler_end_time)
	{
		rumbler_end_time = 0.0f;
		rumbler_start_time = 0.0f;
	}

}

void RumblerManager::StartRumbler(RumblerType type, int _index_controller, float _rumbler_time)
{
	/*rumbler_end_time = Time::GetGameTime() + _rumbler_time;
	rumbler_start_time = Time::GetGameTime();

	if (_index_controller > 0)
		switch (type)
		{
		case RumblerType::RECEIVE_HIT:
			Input::DoRumble(_index_controller, 1.f, 240.0f);
			break;
		case RumblerType::HEAVY_ATTACK:
			Input::DoRumble(_index_controller, 0.8f, 240.0f);
			break;
		case RumblerType::REVIVE:
			Input::DoRumble(_index_controller, 0.5f, 100.0f);
			break;
		case RumblerType::INCREASING:
			increasing += 1 / (_rumbler_time * 100);
			Input::DoRumble(_index_controller, increasing, 200.0f);
			break;
		case RumblerType::DECREASING:
			Input::DoRumble(_index_controller, decreasing, 200.0f);
			decreasing -= 1 / (_rumbler_time * 100);
			break;
		default:
			break;
		}
	else
	{
		StartRumbler(type, 1, _rumbler_time);
		StartRumbler(type, 2, _rumbler_time);
	}*/
}