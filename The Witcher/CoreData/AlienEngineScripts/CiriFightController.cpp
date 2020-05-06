#include "GameManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
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
}

void CiriFightController::Update()
{
	if (game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[0]->transform->GetGlobalPosition()) < 5 || game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[1]->transform->GetGlobalPosition()) < 5) {
		fight_started = true;
		game_object->GetComponent<CiriOriginal>()->state = Boss::BossState::IDLE;
		phase_change = true;
	}
	if (fight_started) {
		switch (phase)
		{
		case 0:
			UpdatePhaseZero();
			break;
		case 1:
			UpdatePhaseOne();
			break;
		case 2:
			UpdatePhaseTwo();
			break;
		case 3:
			UpdatePhaseThree();
			break;
		default:
			break;
		}
	}
}

void CiriFightController::UpdatePhaseZero()
{
	if (phase_0_timer <= phase_0_time) {
		phase_0_timer += Time::GetDT();
		this->GetComponent<ComponentCharacterController>()->Move(float3(0, 0.05f, 0));
	}
	else {
		FinishPhaseZero();
	}
}

void CiriFightController::FinishPhaseZero()
{
	phase++;
	ciri_clones.push_back((Ciri*)(GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition())));
	ciri_clones.push_back((Ciri*)(GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[1]->transform->GetGlobalPosition())));
	this->GetComponent<ComponentCharacterController>()->Move(float3::zero());
}

void CiriFightController::UpdatePhaseOne()
{
}

void CiriFightController::FinishPhaseOne()
{
	ciri_clones.push_back((Ciri*)(GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition())));
}

void CiriFightController::UpdatePhaseTwo()
{
}

void CiriFightController::FinishPhaseTwo()
{
}

void CiriFightController::UpdatePhaseThree()
{
}

void CiriFightController::FinishPhaseThree()
{
}
