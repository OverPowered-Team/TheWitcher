#include "GameManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "CiriFightController.h"
#include "Scores_Data.h"
#include "RumblerManager.h"

CiriFightController::CiriFightController() : Alien()
{
}

CiriFightController::~CiriFightController()
{
	if (material_platform)
		material_platform->material->color = { 1,1,1,1 };
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
	time_platform += rescale_platform_value;

	if (platform != nullptr && !desactived_mid_platform)
	{
		if (time_platform > count_circle&& circle == nullptr)
		{
			std::vector<GameObject*> children = platform->GetChildren();
			for (auto it = children.begin(); it != children.end(); ++it)
			{
				if (strcmp((*it)->GetName(), "extern_circle") == 0)
				{
					circle = (*it);
					time_platform = 0.0;

					if (GameManager::instance->rumbler_manager)
						GameManager::instance->rumbler_manager->StartRumbler(RumblerType::DECREASING, 0, 2.0);
				}
				else if (strcmp((*it)->GetName(), "mid_circle") == 0)
				{
					material_platform = (*it)->GetComponent<ComponentMaterial>();
					break;
				}
			}
		}
		else if (time_platform > count_circle)
		{
			std::vector<GameObject*> children = platform->GetChildren();
			for (auto it = children.begin(); it != children.end(); ++it)
			{
				if (strcmp((*it)->GetName(), "mid_circle") == 0)
				{
					circle->GetComponent<ComponentMaterial>()->material->color = { 1,1,1,1 };
					circle->SetEnable(false);
					circle = (*it);
					desactived_mid_platform = true;
					time_platform = 0.0;

					if (GameManager::instance->rumbler_manager)
						GameManager::instance->rumbler_manager->StartRumbler(RumblerType::DECREASING, 0, 2.0);
					break;
				}
			}
		}
	}

	if (material_platform)
	{
		material_platform->material->color.x += rescale_platform_value * 0.01;
		material_platform->material->color.y -= rescale_platform_value * 0.01;
		material_platform->material->color.z -= rescale_platform_value * 0.01;
	}

	if (circle)
	{
		circle->transform->SetLocalPosition(circle->transform->GetLocalPosition().x, circle->transform->GetLocalPosition().y - rescale_platform_value, circle->transform->GetLocalPosition().z);

		if (time_platform > count_circle)
		{
			material_platform->material->color = { 1,1,1,1 };
			circle->SetEnable(false);
			circle = nullptr;
		}
	}
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
		if (!died) {
			game_object->GetComponent<ComponentAnimator>()->PlayState("Death");
			died = true;
		}
		if (game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[0]->transform->GetGlobalPosition()) < 5 || game_object->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players[1]->transform->GetGlobalPosition()) < 5) {
			FinishPhaseFour();
		}
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
