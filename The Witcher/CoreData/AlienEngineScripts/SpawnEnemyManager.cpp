#include "SpawnEnemyManager.h"
#include "SpawnEnemy.h"

SpawnEnemyManager::SpawnEnemyManager() : Alien()
{
}

SpawnEnemyManager::~SpawnEnemyManager()
{
	enemies_to_spawn.clear();
}

void SpawnEnemyManager::Start()
{
	enemies_to_spawn = GetComponentsInChildren<SpawnEnemy>();
}

void SpawnEnemyManager::Update()
{
	if(!has_spawned)
		CheckForPlayers();
	else if(enemies_to_spawn.empty())
		GameObject::Destroy(game_object);

}

void SpawnEnemyManager::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), spawnerRange, Color::Cyan());
}

void SpawnEnemyManager::CheckForPlayers()
{
	std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(game_object->transform->GetGlobalPosition(), spawnerRange);

	for (int i = 0; i < colliders.size(); ++i)
	{
		if (strcmp(colliders[i]->game_object_attached->GetTag(),"Player") == 0)
		{
			SpawnEnemies();
			has_spawned = true;
			break;
		}
	}
}

void SpawnEnemyManager::SpawnEnemies()
{
	for (int i = 0; i < enemies_to_spawn.size(); ++i)
	{
		enemies_to_spawn[i]->InstantiateEnemy();
	}
}
