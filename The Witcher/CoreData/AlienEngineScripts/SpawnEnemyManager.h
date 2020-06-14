#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class SpawnEnemy;
class Enemy;

class ALIEN_ENGINE_API SpawnEnemyManager : public Alien {

public:

	SpawnEnemyManager();
	virtual ~SpawnEnemyManager();
	
	void Start();
	void Update();
	void OnDrawGizmosSelected();

	void CheckForPlayers();
	std::vector<Enemy*> SpawnEnemies();
	void SpawnEnemiesEnter();

public:
	float spawnerRange = 50.0f;
	float delaySpawnTime = 0.0f;

	bool has_spawned = false;

	std::vector<SpawnEnemy*> enemies_to_spawn;

};

ALIEN_FACTORY SpawnEnemyManager* CreateSpawnEnemyManager() {
	SpawnEnemyManager* spawner = new SpawnEnemyManager();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(spawner->spawnerRange);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(spawner->delaySpawnTime);

	return spawner;
} 
