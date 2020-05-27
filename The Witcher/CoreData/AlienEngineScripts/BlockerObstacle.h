#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class EnemyManager;
class Enemy;
class ALIEN_ENGINE_API BlockerObstacle : public Enemy {

public:
	enum(ObstacleState,
		NONE = -1,
		IDLE,
		DYING,
		DEAD);

	BlockerObstacle();
	virtual ~BlockerObstacle();
	void SetStats(const char* json) override;

	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, PlayerController* player);
	void LookForMyChildren();
	void ManageHealth();
	void OnTriggerEnter(ComponentCollider* collider) override;
	bool IsDead() override;
	void AddChildren(GameObject* g_o);
	void ReleaseChildren();
	void ReleaseMyself(Enemy* en);
public:
	//float health = 0.f;
	//float minimum_health = 0.f;
	ObstacleState state = ObstacleState::NONE;
private:
	std::vector<Enemy*>children_enemies;
	int beginning_enemies = 0.f;
	ComponentRigidBody* r_body = nullptr;
	EnemyManager* manager = nullptr;
	bool has_started = false;
};

ALIEN_FACTORY BlockerObstacle* CreateBlockerObstacle() {
	BlockerObstacle* alien = new BlockerObstacle();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(BlockerObstacle::ObstacleState, alien->state);
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->health);
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->minimum_health);

	return alien;
} 
