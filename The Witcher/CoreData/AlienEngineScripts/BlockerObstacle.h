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
	float GetDamaged(float dmg, PlayerController* player, float3 knockback);
	void LookForMyChildren();
	void ManageHealth();
	bool IsDead() override;
	void AddChildren(GameObject* g_o);
	void ReleaseChildren();
	void ReleaseMyself(Enemy* en);

	void CheckRootHealth();

public:
	//float health = 0.f;
	//float minimum_health = 0.f;
	ObstacleState state = ObstacleState::NONE;
	/*ComponentMaterial* material_3rd = nullptr;
	ComponentMaterial* material_2nd = nullptr;
	ComponentMaterial* material_1st = nullptr;
	ResourceMaterial* resource_mat_org = nullptr;
	ResourceMaterial* resource_mat_3rd = nullptr;
	ResourceMaterial* resource_mat_2nd = nullptr;
	ResourceMaterial* resource_mat_1st = nullptr;*/
	float burnSpeed = 0.5f;

private:
	std::vector<Enemy*>children_enemies;
	int beginning_enemies = 0.f;
	ComponentRigidBody* r_body = nullptr;
	EnemyManager* manager = nullptr;
	bool has_started = false;
	float enemy_die_damage = 0.0f;
	bool root_3rd = false;
	bool root_2nd = false;
	bool root_1st = false;

	ComponentCollider* boxCollider = nullptr;
	std::vector<GameObject*> roots;
};

ALIEN_FACTORY BlockerObstacle* CreateBlockerObstacle() {
	BlockerObstacle* alien = new BlockerObstacle();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(BlockerObstacle::ObstacleState, alien->state);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->burnSpeed);
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->health);
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->minimum_health);

	return alien;
} 
