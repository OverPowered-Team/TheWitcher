#include "EnemyManager.h"
#include "Enemy.h"
#include "NilfgaardSoldier.h"
#include "PlayerController.h"

void EnemyManager::Awake()
{
	GameObject** players = nullptr;
	uint size = GameObject::FindGameObjectsWithTag("Player", &players);
	if (size == 2)
	{
		player1 = players[0];
		player2 = players[1];
	}
	else
		LOG("There's no player or just one in the scene!");
	GameObject::FreeArrayMemory((void***)&players);
}

void EnemyManager::Start()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->player_controllers.push_back(static_cast<PlayerController*>(player1->GetComponentScript("PlayerController")));
		(*item)->player_controllers.push_back(static_cast<PlayerController*>(player2->GetComponentScript("PlayerController")));
		(*item)->StartEnemy();
	}
}

void EnemyManager::Update()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->UpdateEnemy();
	}
}

void EnemyManager::CleanUp()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->CleanUpEnemy();
		(*item)->game_object->ToDelete();
	}
	enemies.clear();
}

Enemy* EnemyManager::CreateEnemy(EnemyType type, const float3& position, ExtraEnumType extra_type)
{
	Enemy* enemy = nullptr;
	switch (type)
	{
	case EnemyType::GHOUL: {
		break; }
	case EnemyType::NILFGAARD_SOLDIER: {
		switch ((NilfgaardSoldier::NilfgaardType)extra_type)
		{
		case NilfgaardSoldier::NilfgaardType::ARCHER: {
			LOG("ARCHER");
			break; }
		case NilfgaardSoldier::NilfgaardType::SWORD: {
			LOG("LARGE_SWORD");
			break; }
		case NilfgaardSoldier::NilfgaardType::SWORD_SHIELD: {
			LOG("SWORD_SHIELD");
			break; }
		default: {
			LOG("Niflgard type wrong")
			break; }
		}
		break; }
	default:
		break;
	}

	if (enemy != nullptr) {
		AddEnemy(enemy);
		enemy->StartEnemy();
	}

	return enemy;
}

void EnemyManager::AddEnemy(Enemy* enemy)
{
	enemies.push_back(enemy);
	if(player1)
		enemy->player_controllers.push_back(static_cast<PlayerController*>(player1->GetComponentScript("PlayerController")));
	if(player2)
		enemy->player_controllers.push_back(static_cast<PlayerController*>(player2->GetComponentScript("PlayerController")));
}

void EnemyManager::DeleteEnemy(Enemy* enemy)
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		if (*item == enemy) {
			enemy->CleanUpEnemy();
			enemy->game_object->ToDelete();
			enemies.erase(item);
			break;
		}
	}
}

