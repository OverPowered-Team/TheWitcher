#include "EnemyManager.h"
#include "Enemy.h"
#include "NilfgaardSoldier.h"
#include "Drowned.h"
#include "PlayerController.h"

void EnemyManager::Awake()
{
	auto players = GameObject::FindGameObjectsWithTag("Player");
	if (players.size() == 2)
	{
		player1 = players[0];
		player2 = players[1];
	}
	else
		LOG("There's no player or just one in the scene!");
}

void EnemyManager::Start()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->player_controllers.push_back(player1->GetComponent<PlayerController>());
		(*item)->player_controllers.push_back(player2->GetComponent<PlayerController>());
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
	case EnemyType::DROWNED:
		switch ((Drowned::DrownedType)extra_type)
		{
		case Drowned::DrownedType::GRAB: {
			LOG("Drowned Grab");
			break; }
		case Drowned::DrownedType::RANGE: {
			LOG("Drowned Range");
			break; }
		default: {
			LOG("Drowned type wrong")
				break; }
		}
		break;
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
		enemy->player_controllers.push_back(player1->GetComponent<PlayerController>());
	if(player2)
		enemy->player_controllers.push_back(player2->GetComponent<PlayerController>());
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

