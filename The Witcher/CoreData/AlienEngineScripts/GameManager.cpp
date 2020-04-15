#include "GameManager.h"

GameManager::GameManager() : Alien()
{
}

GameManager::~GameManager()
{
}

void GameManager::Awake()
{
	manager = this;

	event_manager = (EventManager*)game_object->GetComponentScript("EventManager");
	enemy_manager = (EnemyManager*)game_object->GetComponentScript("EnemyManager");
	dialogue_manager = (DialogueManager*)game_object->GetComponentScript("DialogueManager");
	player_manager = (PlayerManager*)game_object->GetComponentScript("PlayerManager");
	relic_manager = (RelicManager*)game_object->GetComponentScript("RelicManager");
}

void GameManager::Start()
{
}

void GameManager::Update()
{
}

void GameManager::Rumbler(RumblerType type, int index_controller)
{
	if (index_controller > 0)
		switch (type)
		{
		case RumblerType::RECEIVE_HIT:
			Input::DoRumble(index_controller, 1.f, 240.0f);
			break;
		case RumblerType::REVIVE:
			Input::DoRumble(index_controller, 0.5f, 100.0f);
			break;
		default:
			break;
		}
	else
	{
		Rumbler(type, 1);
		Rumbler(type, 2);
	}
}