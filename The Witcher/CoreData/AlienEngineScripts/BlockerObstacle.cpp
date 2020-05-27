#include "BlockerObstacle.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "SpawnEnemyManager.h"

BlockerObstacle::BlockerObstacle() : Enemy()
{
}

BlockerObstacle::~BlockerObstacle()
{
}

void BlockerObstacle::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
	if (stat)
	{
		stats["Health"] = Stat("Health", stat->GetNumber("Health"));
		stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
	}

	JSONfilepack::FreeJSON(stat);
}

void BlockerObstacle::StartEnemy()
{
	type = EnemyType::BLOCKER_OBSTACLE;
	Enemy::StartEnemy();
	state = ObstacleState::IDLE;
	children_enemies = this->game_object->GetChild("ChildEnemies")->GetComponentsInChildren<Enemy>();
	manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();
}

void BlockerObstacle::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	//LOG("MY LIFE: %f", stats["Health"].GetValue());
	switch (state)
	{
	case ObstacleState::IDLE: 
	{
		if (distance < stats["VisionRange"].GetValue() && !has_started)
			LookForMyChildren();
	}
	break;
	case ObstacleState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5.0f);
		if(!children_enemies.empty())
			ReleaseChildren();
		state = ObstacleState::DEAD;
	}
	break;
	case ObstacleState::DEAD:
		break;
	default:
		LOG("There's no state");
		break;
	}
}

void BlockerObstacle::CleanUpEnemy()
{
	children_enemies.clear();
}

float BlockerObstacle::GetDamaged(float dmg, PlayerController* player, float3 knockback)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	if (stats["Health"].GetValue() <= 0.f)
		state = ObstacleState::DYING;

	last_player_hit = player;
	return aux_health - stats["Health"].GetValue();
}

void BlockerObstacle::LookForMyChildren()
{
	for (auto iter = children_enemies.begin(); iter != children_enemies.end(); ++iter) {
		if (!(*iter)->IsState("Idle")) {
			for (auto iter2 = children_enemies.begin(); iter2 != children_enemies.end(); ++iter2) {
				(*iter2)->is_obstacle = true;
			}
			has_started = true;
			GameObject* spawner = game_object->GetChild("SpawnerManager");
			if (spawner)
				spawner->GetComponent<SpawnEnemyManager>()->SpawnEnemies();
			else
				LOG("No spawner");
		}
	}
}

void BlockerObstacle::ManageHealth()
{
	stats["Health"].DecreaseStat(200.f);
	if (children_enemies.size() <= 0 || stats["Health"].GetValue() <= 0) {
		state = ObstacleState::DYING;
		LOG("ESTOY MUERTO");
	}
}

bool BlockerObstacle::IsDead()
{
	return (state == ObstacleState::DEAD ? true : false);
}

void BlockerObstacle::AddChildren(GameObject* g_o)
{
	Enemy* en = game_object->GetComponent<Enemy>();
	g_o->SetNewParent(game_object);
	en->is_obstacle = true;
	children_enemies.push_back(en);
}

void BlockerObstacle::ReleaseChildren()
{
	for (auto it = children_enemies.begin(); it != children_enemies.end(); ++it)
	{
		(*it)->game_object->SetNewParent(game_object->parent->parent);
		(*it)->is_obstacle = false;
	}
}

void BlockerObstacle::ReleaseMyself(Enemy* en)
{
	auto iter = std::find(children_enemies.begin(), children_enemies.end(), en);
	children_enemies.erase(iter);
	ManageHealth();
	LOG("I HAVE THIS CHILDREN: %i", children_enemies.size());
}
