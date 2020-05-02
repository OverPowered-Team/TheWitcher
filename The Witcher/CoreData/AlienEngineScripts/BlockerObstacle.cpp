#include "BlockerObstacle.h"
#include "EnemyManager.h"

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

	LOG("MY LIFE: %d", stats["Health"].GetValue());
}

void BlockerObstacle::StartEnemy()
{
	type = EnemyType::BLOCKER_OBSTACLE;
	Enemy::StartEnemy();
}

void BlockerObstacle::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	LOG("MY LIFE: %d", stats["Health"].GetValue());
	switch (state)
	{
	case Enemy::EnemyState::IDLE:
		if (distance < stats["VisionRange"].GetValue())
			//LookForMyChildren();
		break;
	case Enemy::EnemyState::MOVE:
		break;
	case Enemy::EnemyState::BLOCK:
		break;
	case Enemy::EnemyState::ATTACK:
		break;
	case Enemy::EnemyState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = EnemyState::DEAD;
		break;
	}
	default:
		LOG("There's no state");
		break;
	}
}

void BlockerObstacle::CleanUpEnemy()
{
	children_enemies.clear();
}

void BlockerObstacle::Start()
{
	children_enemies = this->game_object->GetChildren();
	LOG("I HAVE THIS CHILDREN: %i", children_enemies.size());
	LOG("NEW HEALTH: %f", health);
}

void BlockerObstacle::Update()
{
	//THIS HAS TO BE WITHIN A RANGE OR RADIUS 
	
}

void BlockerObstacle::CleanUp()
{

}

void BlockerObstacle::LookForMyChildren()
{
	auto iter = children_enemies.begin();
	while(iter != children_enemies.end())
	{
		if ((*iter)->GetComponent<Enemy>()->state == Enemy::EnemyState::DEAD)
		{
			iter = children_enemies.erase(iter);
			ManageHealth();
			LOG("I HAVE THIS CHILDREN: %i", children_enemies.size());
		}
		else {
			++iter;
		}
	}
}

void BlockerObstacle::ManageHealth()
{
	if (health <= minimum_health || children_enemies.size() <= 0)
		return;
	//FOR NOW THIS WILL BE A THREE RULE CALCULATION
	health -= ((health - minimum_health) / children_enemies.size());
	LOG("NEW HEALTH: %f", health);
}

