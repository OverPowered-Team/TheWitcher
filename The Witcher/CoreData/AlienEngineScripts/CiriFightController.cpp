#include "GameManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "CiriFightController.h"
#include "Scores_Data.h"

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
	if (scream_cd_timer <= ciri_clones_scream_cd) {
		scream_cd_timer += Time::GetDT();
	}
	else {
		can_mini_scream = true;
	}
	if ((game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[0]->transform->GetGlobalPosition()) < 5 || game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[1]->transform->GetGlobalPosition()) < 5) && !fight_started) {
		fight_started = true;
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
		case 4:
			UpdatePhaseFour();
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
	GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());
	GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[1]->transform->GetGlobalPosition());
	this->GetComponent<ComponentCharacterController>()->Move(float3::zero());
}

void CiriFightController::UpdatePhaseOne()
{
}

void CiriFightController::FinishPhaseOne()
{
	GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());
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

void CiriFightController::FinishPhaseFour()
{
	Scores_Data::player1_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
	Scores_Data::player2_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;
	SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::FADE);
	Destroy(game_object);
}

void CiriFightController::UpdatePhaseFour()
{
	if (phase_4_timer <= phase_4_time) {
		phase_4_timer += Time::GetDT();
		this->GetComponent<ComponentCharacterController>()->Move(float3(0, -0.05f, 0));
	}
	else {
		FinishPhaseFour();
	}
}

void CiriFightController::ChangeToPhase2()
{
	phase = 2;
	phase_change = true;
	GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());
}

void CiriFightController::ChangeToPhase3()
{
	phase = 3;
	phase_change = true;
}

void CiriFightController::ChangeToPhase4()
{
	phase = 4;
}

void CiriFightController::OnCloneDead(GameObject* clone)
{
	clones_dead++;
	game_object->GetComponent<CiriOriginal>()->GetDamaged(clone_dead_damage);
	if (clones_dead == 2) {
		ChangeToPhase2();
	}
	else if (clones_dead == 5)
		ChangeToPhase3();
	else if (clones_dead == 8)
		ChangeToPhase4();
	
	if(clones_dead < 6)
		GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());

	LOG("CURRENT PHASE %i", phase);
}
