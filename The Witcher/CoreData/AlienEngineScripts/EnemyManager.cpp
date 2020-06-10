#include "EnemyManager.h"
#include "Enemy.h"
#include "NilfgaardSoldier.h"
#include "Ghoul.h"
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

	if (Input::GetKeyDown(SDL_SCANCODE_E))
	{
		ChangeIsHitInmune();
		LOG("Is hit enemies: %i", is_hit_inmune);
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

Enemy* EnemyManager::CreateEnemy(EnemyType type, const float3& position, ExtraEnumType extra_type, GameObject* parent)
{
	Enemy* enemy = nullptr;
	switch (type)
	{
	case EnemyType::GHOUL: {
		switch ((Ghoul::GhoulType)extra_type)
		{
		case Ghoul::GhoulType::ORIGINAL:
			enemy = GameObject::Instantiate(ghoul_original, position, false, parent)->GetComponent<Enemy>();
			LOG("GHOUL_ORIGINAL");
			break;
		case Ghoul::GhoulType::DODGE:
			enemy = GameObject::Instantiate(ghoul_dodge, position, false, parent)->GetComponent<Enemy>();
			LOG("GHOUL_DODGE");
			break;
		case Ghoul::GhoulType::MINI:
			enemy = GameObject::Instantiate(ghoul_mini, position, false, parent)->GetComponent<Enemy>();
			LOG("GHOUL_MINI");
			break;
		}
		break; }
	case EnemyType::NILFGAARD_SOLDIER: {
		switch ((NilfgaardSoldier::NilfgaardType)extra_type)
		{
		case NilfgaardSoldier::NilfgaardType::SWORD: {
			enemy = GameObject::Instantiate(nilf_melee, position, false, parent)->GetComponent<Enemy>();
			LOG("SWORD");
			break; }
		case NilfgaardSoldier::NilfgaardType::ARCHER: {
			enemy = GameObject::Instantiate(nilf_range, position, false, parent)->GetComponent<Enemy>();
			LOG("ARCHER");
			break; }
		case NilfgaardSoldier::NilfgaardType::SWORD_SHIELD: {
			enemy = GameObject::Instantiate(nilf_shield, position, false, parent)->GetComponent<Enemy>();
			LOG("SHIELD");
			break; }
		default: {
			LOG("Niflgaard type wrong")
				break; }
		}
		break; }
	case EnemyType::DROWNED: {
		switch ((Drowned::DrownedType)extra_type)
		{
		case Drowned::DrownedType::RANGE: {
			enemy = GameObject::Instantiate(drowned_range, position, false, parent)->GetComponent<Enemy>();
			LOG("Drowned Range");
			break; }
		default: {
			LOG("Drowned type wrong")
				break; }
		}
		break;
	}
	case EnemyType::CIRI_CLONE: {
		enemy = GameObject::Instantiate(ciri_clone, position, true, parent)->GetComponent<Enemy>();
		enemy->GetComponent<ComponentCharacterController>()->SetPosition(position);
		break;
	}
	default:
		break;
	}

	if (enemy != nullptr) {
		enemy->player_controllers.push_back(player1->GetComponent<PlayerController>());
		enemy->player_controllers.push_back(player2->GetComponent<PlayerController>());
		enemy->StartEnemy();
	}

	return enemy;
}

void EnemyManager::AddEnemy(Enemy* enemy)
{
	enemies.push_back(enemy);
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

void EnemyManager::ChangeIsHitInmune()
{
	is_hit_inmune = !is_hit_inmune;

	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->is_hit_inmune = is_hit_inmune;
	}
}

const std::vector<Enemy*>& EnemyManager::GetEnemies()
{
	return (enemies);
}

