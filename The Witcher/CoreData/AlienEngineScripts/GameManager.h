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
	HEAVY_ATTACK,
	REVIVE,
	INCREASING,
	DECREASING,
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

	//Rumbler
	void Rumbler(RumblerType type, int index_controller = 0, float rumbler_time = 0.0f);
	float increasing = 0.0f;
	float decreasing = 1.0f;
	float rumbler_start_time = 0.0f;
	float rumbler_end_time = 0.0f;
	int index_controller = 0;

};
GameManager* GameManager::manager = nullptr;

ALIEN_FACTORY GameManager* CreateGameManager() {
	GameManager* alien = new GameManager();
	// To show in inspector here

	return alien;
}

