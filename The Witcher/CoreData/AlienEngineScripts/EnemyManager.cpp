#include "EnemyManager.h"

EnemyManager::EnemyManager() : Alien()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Start()
{
	for (auto it_enemy = enemies.begin(); it_enemy != enemies.end(); it_enemy++)
	{
		(*it_enemy)->StartEnemy();
	}
}

void EnemyManager::Update()
{
}