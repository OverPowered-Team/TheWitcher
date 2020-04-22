#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class EnemyManager;
class DialogueManager;
class PlayerManager;
class RelicManager;
class RumblerManager;

class ALIEN_ENGINE_API GameManager : public Alien {

public:
	virtual ~GameManager();

	EventManager* event_manager = nullptr;
	EnemyManager* enemy_manager = nullptr;
	DialogueManager* dialogue_manager = nullptr;
	PlayerManager* player_manager = nullptr;
	RelicManager* relic_manager = nullptr;
	RumblerManager* rumbler_manager = nullptr;

	void Awake();
	void Start();
	void Update();

	static GameManager* GetInstance()
	{
		if(!instance)
			instance = new GameManager();

		return instance;
	}
private:
	GameManager();
	static GameManager* instance;

};
GameManager* GameManager::instance = 0;

ALIEN_FACTORY GameManager* CreateGameManager() {
	GameManager* alien = GameManager::GetInstance();
	// To show in inspector here

	return alien;
}

