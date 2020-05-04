#include "GameManager.h"
#include "EnemyManager.h"
#include "CiriFightController.h"

CiriFightController::CiriFightController() : Alien()
{
}

CiriFightController::~CiriFightController()
{
}

void CiriFightController::Start()
{
	clone_positions = game_object->GetChild("ClonePositions")->GetChildren();
	ciri_clones.push_back((Ciri*)(GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition())));
	ciri_clones.push_back((Ciri*)(GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[1]->transform->GetGlobalPosition())));
}

void CiriFightController::Update()
{
	if (scream_cd_timer < ciri_clones_scream_cd) {
		scream_cd_timer += Time::GetDT();
	}
	else {
		can_mini_scream = true;
		scream_cd_timer = 0;
	}
}