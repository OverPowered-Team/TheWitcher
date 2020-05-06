#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;
enum class EnemyType;
typedef int ExtraEnumType;

class ALIEN_ENGINE_API EnemyManager : public Alien {

public:

	EnemyManager() {}
	virtual ~EnemyManager() {}
	
	void Awake();
	void Start();
	void Update();
	void CleanUp();

	Enemy* CreateEnemy(EnemyType type, const float3& position, ExtraEnumType extra_type = -1, GameObject* parent = nullptr);

	void AddEnemy(Enemy* enemy);
	void DeleteEnemy(Enemy* enemy);
	const std::vector<Enemy*>&GetEnemies();

public:
	GameObject* player1 = nullptr;
	GameObject* player2 = nullptr;

	Prefab nilf_melee;
	Prefab nilf_range;
	Prefab nilf_shield;

	Prefab ghoul_original;
	Prefab ghoul_dodge;

	Prefab drowned_range;
	Prefab drowned_grab;

private:
	std::vector<Enemy*> enemies;

};

ALIEN_FACTORY EnemyManager* CreateEnemyManager() {
	EnemyManager* manager = new EnemyManager();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_PREFAB(manager->nilf_melee);
	SHOW_IN_INSPECTOR_AS_PREFAB(manager->nilf_range);
	SHOW_IN_INSPECTOR_AS_PREFAB(manager->nilf_shield);

	SHOW_IN_INSPECTOR_AS_PREFAB(manager->ghoul_original);
	SHOW_IN_INSPECTOR_AS_PREFAB(manager->ghoul_dodge);

	SHOW_IN_INSPECTOR_AS_PREFAB(manager->drowned_range);
	SHOW_IN_INSPECTOR_AS_PREFAB(manager->drowned_grab);

	return manager;
} 
