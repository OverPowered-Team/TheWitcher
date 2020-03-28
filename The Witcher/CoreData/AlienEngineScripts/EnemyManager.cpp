#include "EnemyManager.h"

EnemyManager::EnemyManager() : Alien()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Start()
{
	GameObject** get_enemies = nullptr;
	uint size;
	size = GameObject::FindGameObjectsWithTag("Enemy", &get_enemies);
	for (int i = 0; i < size; i++) {
		GameObject* g = get_enemies[i];
		enemies.push_back((Enemy*)g->GetComponentScript("NilfgaardSoldier"));
	}
	GameObject::FreeArrayMemory((void***)&get_enemies);

	/*for (auto it_enemy = enemies.begin(); it_enemy != enemies.end(); it_enemy++)
	{
		(*it_enemy)->StartEnemy();
	}*/
}

void EnemyManager::Update()
{
	/*for (auto it_enemy = enemies.begin(); it_enemy != enemies.end(); it_enemy++)
	{
		(*it_enemy)->UpdateEnemy();
	}*/
}