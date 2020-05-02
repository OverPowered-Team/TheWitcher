#include "BlockerObstacle.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Enemy.h"

BlockerObstacle::BlockerObstacle() : Alien()
{
}

BlockerObstacle::~BlockerObstacle()
{
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
	LookForMyChildren();
	
}

void BlockerObstacle::CleanUp()
{
	children_enemies.clear();
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

void BlockerObstacle::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0) {
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player)
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			health -= dmg_received;
			//player->OnHit(this, GetDamaged(dmg_received, player));
		}
	}
}

