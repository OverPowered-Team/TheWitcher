#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API SpawnEnemy : public Alien {

public:

	SpawnEnemy();
	virtual ~SpawnEnemy();

	void InstantiateEnemy();

public:
	EnemyType enemy_type = EnemyType::NONE;
	int extra_type;

};

ALIEN_FACTORY SpawnEnemy* CreateSpawnEnemy() {
	SpawnEnemy* spawner = new SpawnEnemy();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(EnemyType, spawner->enemy_type);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_INT(spawner->extra_type);

	return spawner;
} 
