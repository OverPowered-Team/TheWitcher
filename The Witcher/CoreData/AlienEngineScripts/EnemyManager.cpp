#include "EnemyManager.h"
#include "Enemy.h"
#include "NilfgaardSoldier.h"

void EnemyManager::Start()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
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
	/*for (auto it_enemy = enemies.begin(); it_enemy != enemies.end(); it_enemy++)
	{
		(*it_enemy)->StartEnemy();
	}*/
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
		case NilfgaardSoldier::NilfgaardType::SPEAR: {
			LOG("SPEAR");
			break; }
		case NilfgaardSoldier::NilfgaardType::ARCHER: {
			LOG("ARCHER");
			break; }
		case NilfgaardSoldier::NilfgaardType::LARGE_SWORD: {
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
	enemy->player_1 = player1;
	enemy->player_2 = player2;
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

