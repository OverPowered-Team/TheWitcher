#include "GameManager.h"
#include "SpawnEnemy.h"
#include "EnemyManager.h"
#include "Enemy.h"

SpawnEnemy::SpawnEnemy() : Alien()
{
}

SpawnEnemy::~SpawnEnemy()
{
}

void SpawnEnemy::InstantiateEnemy()
{
	Enemy* enemy = GameManager::instance->enemy_manager->CreateEnemy(enemy_type, game_object->transform->GetGlobalPosition(), (ExtraEnumType)extra_type);
	enemy->character_ctrl->SetPosition(game_object->transform->GetGlobalPosition());
	Destroy(game_object);
}
