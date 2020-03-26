#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API EnemyManager : public Alien {

public:

	EnemyManager();
	virtual ~EnemyManager();
	
	void Start();
	void Update();

public:
	std::vector<Enemy*> enemies;

};

ALIEN_FACTORY EnemyManager* CreateEnemyManager() {
	EnemyManager* alien = new EnemyManager();
	// To show in inspector here

	return alien;
} 
