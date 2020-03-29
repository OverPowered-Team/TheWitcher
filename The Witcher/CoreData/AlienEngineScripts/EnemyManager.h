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
	
	void Start();
	void Update();
	void CleanUp();

	void CreateEnemy(EnemyType type, const float3& position, ExtraEnumType extra_type = -1);

	void AddEnemy(Enemy* enemy);
	void DeleteEnemy(Enemy* enemy);

private:
	std::vector<Enemy*> enemies;


};

ALIEN_FACTORY EnemyManager* CreateEnemyManager() {
	EnemyManager* alien = new EnemyManager();
	// To show in inspector here

	return alien;
} 
