#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class EnemyManager;
class DialogueManager;
class PlayerManager;

class ALIEN_ENGINE_API GameManager : public Alien {

public:
	GameManager();
	virtual ~GameManager();

	EventManager* event_manager = nullptr;
	EnemyManager* enemy_manager = nullptr;
	DialogueManager* dialogue_manager = nullptr;
	PlayerManager* player_manager = nullptr;

	void Awake();
	void Start();
	void Update();
};
extern GameManager* Game_Manager = nullptr;

ALIEN_FACTORY GameManager* CreateGameManager() {
	GameManager* alien = new GameManager();
	// To show in inspector here

	return alien;
}

