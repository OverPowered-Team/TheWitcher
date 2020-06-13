#include "BlockerObstacle.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "SpawnEnemyManager.h"
#include "GameManager.h"

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
	state = ObstacleState::IDLE;
	children_enemies = this->game_object->GetChild("ChildEnemies")->GetComponentsInChildren<Enemy>();
	manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();

	SetStats("blockerobstacle");

	start_pos = game_object->GetComponent<ComponentTransform>()->GetGlobalPosition();
	is_hit_inmune = GameManager::instance->enemy_manager->is_hit_inmune;

	boxCollider = GetComponent<ComponentBoxCollider>();
	audio_emitter = GetComponent<ComponentAudioEmitter>();

	roots = game_object->GetChild("Mesh")->GetChild("Roots")->GetChildren();

	// Root Material
	// 3rd Roots
	material_3rd = roots[2]->GetChild(0)->GetComponent<ComponentMaterial>();
	material_3rd->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;

	//// 2nd Roots
	material_2nd = roots[1]->GetChild(0)->GetComponent<ComponentMaterial>();
	material_2nd->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;

	//// 1st Roots
	material_1st = roots[0]->GetChild(0)->GetComponent<ComponentMaterial>();
	material_1st->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;


	

}

void BlockerObstacle::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case ObstacleState::IDLE: 
	{
		if (distance < stats["VisionRange"].GetValue() && !has_started)
			LookForMyChildren();

		if (root_3rd)
		{
			material_3rd->material->shaderInputs.dissolveFresnelShaderProperties.burn -= burnSpeed * Time::GetDT();

			if (material_3rd->material->shaderInputs.dissolveFresnelShaderProperties.burn <= 0)
			{
				roots[2]->SetEnable(false);
				root_3rd = false;
			}
		}
		else if (root_2nd)
		{
			material_2nd->material->shaderInputs.dissolveFresnelShaderProperties.burn -= burnSpeed * Time::GetDT();

			if (material_2nd->material->shaderInputs.dissolveFresnelShaderProperties.burn <= 0)
			{
				roots[1]->SetEnable(false);
				root_2nd = false;
			}

		}
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
		if (root_1st)
		{
			material_1st->material->shaderInputs.dissolveFresnelShaderProperties.burn -= burnSpeed * Time::GetDT();

			if (material_1st->material->shaderInputs.dissolveFresnelShaderProperties.burn <= 0)
			{
				roots[0]->SetEnable(false);
				root_1st = false;
			}

		}
		break;
	default:
		LOG("There's no state");
		break;
	}
}

void BlockerObstacle::CleanUpEnemy()
{
	children_enemies.clear();

	material_3rd->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;
	material_2nd->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;
	material_1st->material->shaderInputs.dissolveFresnelShaderProperties.burn = 1;

	material_3rd->material->renderMode = 0;
	material_2nd->material->renderMode = 0;
	material_1st->material->renderMode = 0;

}

float BlockerObstacle::GetDamaged(float dmg, PlayerController* player, float3 knockback)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);
	CheckRootHealth();

	if (stats["Health"].GetValue() <= 0.f)
	{
		state = ObstacleState::DYING;
		root_1st = true;
		material_1st->material->renderMode = 1;
	}

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

			enemy_die_damage = stats["Health"].GetMaxValue() / children_enemies.size();
		}
	}
}

void BlockerObstacle::ManageHealth()
{
	stats["Health"].DecreaseStat(enemy_die_damage);
	CheckRootHealth();
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

void BlockerObstacle::CheckRootHealth()
{
	float current_health = stats["Health"].GetValue();
	float max_health = stats["Health"].GetMaxValue();
	float two_third_life = max_health * 0.66f;
	float one_third_life = max_health * 0.33f;

	LOG("Two: %f", two_third_life);
	LOG("One: %f", one_third_life);

	if (current_health <= two_third_life && current_health > one_third_life&& roots[2]->IsEnabled())
	{
		material_3rd->material->renderMode = 1;
		root_3rd = true;
	}
		
	else if (current_health <= one_third_life && roots[1]->IsEnabled())
	{
		material_2nd->material->renderMode = 1;
		root_2nd = true;
	}
		

}
