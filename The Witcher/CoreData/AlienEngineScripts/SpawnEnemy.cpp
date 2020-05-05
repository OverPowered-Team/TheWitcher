#include "GameManager.h"
#include "SpawnEnemy.h"
#include "EnemyManager.h"

SpawnEnemy::SpawnEnemy() : Alien()
{
}

SpawnEnemy::~SpawnEnemy()
{
}

void SpawnEnemy::InstantiateEnemy()
{
	GameManager::instance->enemy_manager->CreateEnemy(enemy_type, transform->GetGlobalPosition(), (ExtraEnumType)extra_type);
	Destroy(game_object);
}
