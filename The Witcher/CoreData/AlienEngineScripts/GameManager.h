#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class EnemyManager;
class DialogueManager;
class PlayerManager;
class RelicManager;
class RumblerManager;
class EffectsFactory;
class ParticlePool;
class Scores_Data;

class ALIEN_ENGINE_API GameManager : public Alien {

public:
	GameManager();
	virtual ~GameManager();

	static GameManager* instance;

	EventManager* event_manager = nullptr;
	EnemyManager* enemy_manager = nullptr;
	DialogueManager* dialogue_manager = nullptr;
	PlayerManager* player_manager = nullptr;
	RelicManager* relic_manager = nullptr;
	RumblerManager* rumbler_manager = nullptr;
	EffectsFactory* effects_factory = nullptr;
	ParticlePool* particle_pool = nullptr;

	void Awake();
	void Start();
	void Update();

	void PrepareDataNextScene(bool dead);
};
GameManager* GameManager::instance = 0;

ALIEN_FACTORY GameManager* CreateGameManager() {
	GameManager* alien = new GameManager();
	GameManager::instance = alien;
	// To show in inspector here

	return alien;
}

