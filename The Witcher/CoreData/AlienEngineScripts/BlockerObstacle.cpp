#include "BlockerObstacle.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"

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

	LOG("MY LIFE: %f", stats["Health"].GetValue());
}

void BlockerObstacle::StartEnemy()
{
	type = EnemyType::BLOCKER_OBSTACLE;
	Enemy::StartEnemy();
	state = ObstacleState::IDLE;
	children_enemies = this->game_object->GetChildren();
}

void BlockerObstacle::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	LOG("MY LIFE: %f", stats["Health"].GetValue());
	switch (state)
	{
	case ObstacleState::IDLE: {
		//if (distance < stats["VisionRange"].GetValue())
			//LookForMyChildren();
			break;
	}
	case ObstacleState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = ObstacleState::DEAD;
		break;
	}
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

float BlockerObstacle::GetDamaged(float dmg, PlayerController* player)
{
	float damage = Enemy::GetDamaged(dmg, player);
	if (stats["Health"].GetValue() == 0.0F) {

		//animator->SetBool("dead", true);
		//OnDeathHit();
		state = ObstacleState::DYING;
		/*audio_emitter->StartSound("GhoulDeath");*/
		player->OnEnemyKill();
	}
	return damage;
}

void BlockerObstacle::LookForMyChildren()
{
	auto iter = children_enemies.begin();
	while(iter != children_enemies.end())
	{
		if ((*iter)->GetComponent<Enemy>()->IsDead())
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
	//if (health <= minimum_health || children_enemies.size() <= 0)
	//	return;
	////FOR NOW THIS WILL BE A THREE RULE CALCULATION
	//health -= ((health - minimum_health) / children_enemies.size());
	//LOG("NEW HEALTH: %f", health);
}

void BlockerObstacle::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != ObstacleState::DEAD) {
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player)
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			player->OnHit(this, GetDamaged(dmg_received, player));

			if (state == ObstacleState::DYING)
				player->OnEnemyKill();

			HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
		}
	}
}