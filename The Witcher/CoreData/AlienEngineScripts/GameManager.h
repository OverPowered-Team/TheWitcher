#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class EnemyManager;
class DialogueManager;
class PlayerManager;
class RelicManager;

enum (RumblerType,
	RECEIVE_HIT,
	REVIVE,
	NONE
	);

class ALIEN_ENGINE_API GameManager : public Alien {

public:
	GameManager();
	virtual ~GameManager();

	static GameManager* manager;
	EventManager* event_manager = nullptr;
	EnemyManager* enemy_manager = nullptr;
	DialogueManager* dialogue_manager = nullptr;
	PlayerManager* player_manager = nullptr;
	RelicManager* relic_manager = nullptr;

	void Awake();
	void Start();
	void Update();

	void Rumbler(RumblerType type, int index_controller = 0);

};
GameManager* GameManager::manager = nullptr;

ALIEN_FACTORY GameManager* CreateGameManager() {
	GameManager* alien = new GameManager();
	// To show in inspector here

	return alien;
}

