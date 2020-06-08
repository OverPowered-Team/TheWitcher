#include "GameManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "CiriFightController.h"
#include "Scores_Data.h"
#include "RumblerManager.h"
#include "CameraMovement.h"
#include "ParticlePool.h"
#include "UI_DamageCount.h"

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

	if (platform)
		material_platform = (*platform->GetChildren().begin())->GetComponent<ComponentMaterial>();

	if (wall)
	{
		std::vector<GameObject*> children = wall->GetChildren();
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			if (strcmp((*it)->GetName(), "tube_collider") == 0 || strcmp((*it)->GetName(), "tube_door") == 0)
			{
			}
			else if ((*it)->IsEnabled())
				rings_enabled.push_back(*it);
			else
				rings_disabled.push_back(*it);
		}
		position_respawn = float3(children.back()->transform->GetLocalPosition());
	}
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
		GameManager::instance->particle_pool->GetInstance("ciri_torbellino", transform->GetGlobalPosition());
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
	phase = 2;
	phase_change = true;
	GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());
}

void CiriFightController::UpdatePhaseTwo()
{
	MoveWall();
}

void CiriFightController::FinishPhaseTwo()
{
	time_platform = 0.0f;
	changing_platform = false;
	phase = 3;
	phase_change = true;
}

void CiriFightController::UpdatePhaseThree()
{
	MoveWall();
	if (!first_wall_door)
		UpdatePlatform();
	TransportPlayer();
}

void CiriFightController::FinishPhaseThree()
{
	phase = 4;
}

void CiriFightController::FinishPhaseFour()
{
	Scores_Data::won_level2 = true;
	Scores_Data::last_scene = SceneManager::GetCurrentScene();
	Scores_Data::player1_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
	Scores_Data::player2_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;
	GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>()->AddRemainingComboPoints();
	Scores_Data::player1_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->relics;
	Scores_Data::player2_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->relics;
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

void CiriFightController::OnCloneDead(GameObject* clone)
{
	clones_dead++;
	game_object->GetComponent<CiriOriginal>()->GetDamaged(clone_dead_damage);
	if (clones_dead == 2) {
		FinishPhaseOne();
	}
	else if (clones_dead == 5)
		FinishPhaseTwo();
	else if (clones_dead == 8)
		FinishPhaseThree();
	
	if(clones_dead < 6)
		GameManager::instance->enemy_manager->CreateEnemy(EnemyType::CIRI_CLONE, clone_positions[0]->transform->GetGlobalPosition());
}


void CiriFightController::MoveWall()
{
	time_platform += rescale_platform_value;
	if (wall != nullptr)
	{
		wall->transform->AddPosition({ 0, -rescale_platform_value, 0 });

		if (time_platform > count_circle && !first_wall_door)
		{
			int random_index = Random::GetRandomIntBetweenTwo(1, 4);

			int i = 1;
			for (auto it = rings_disabled.begin(); it != rings_disabled.end(); ++it)
			{
				if (i == random_index)
				{
					(*it)->SetEnable(true);
					position_respawn = position_respawn + float3(0, 57.5f, 0);
					(*it)->transform->SetLocalPosition(position_respawn);
					rings_enabled.push_back(*it);
					rings_disabled.erase(it);
					break;
				}
				i++;
			}
			(*rings_enabled.begin())->SetEnable(false);
			rings_disabled.push_back(*rings_enabled.begin());
			rings_enabled.erase(rings_enabled.begin());
			time_platform = 0.0f;
			changing_platform = true;
		}
		else if (time_platform > count_circle&& first_wall_door)
		{
			first_wall_door = false;
			time_platform = 0.0f;
		}
	}
}

void CiriFightController::ScaleWall()
{
	//Escalar wall y collider
	if (wall)
		wall->transform->SetLocalScale(wall->transform->GetLocalScale().x - 0.2, wall->transform->GetLocalScale().y, wall->transform->GetLocalScale().z - 0.2);
}

void CiriFightController::UpdatePlatform()
{
	if (platform != nullptr && !desactived_mid_platform)
	{
		if (changing_platform && circle == nullptr)
		{
			std::vector<GameObject*> children = platform->GetChildren();
			for (auto it = children.begin(); it != children.end(); ++it)
			{
				if (strcmp((*it)->GetName(), "extern_circle") == 0)
				{
					circle = (*it);
					changing_platform = false;

					if (GameManager::instance->rumbler_manager)
						GameManager::instance->rumbler_manager->StartRumbler(RumblerType::DECREASING, 0, 2.0);
				}
				else if (strcmp((*it)->GetName(), "mid_circle") == 0)
				{
					if (material_platform)
						material_platform->material->color = { 1,1,1,1 };
					material_platform = (*it)->GetComponent<ComponentMaterial>();
				}
			}
		}
		else if (changing_platform)
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
					changing_platform = false;
					ScaleWall();
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
		circle->transform->SetLocalPosition(circle->transform->GetLocalPosition().x, circle->transform->GetLocalPosition().y - (rescale_platform_value * 2), circle->transform->GetLocalPosition().z);

		if (changing_platform)
		{
			if (material_platform)
				material_platform->material->color = { 1,1,1,1 };
			circle->SetEnable(false);
			circle = nullptr;
			ScaleWall();
		}
	}
}

void CiriFightController::TransportPlayer()
{
	// tp y da�o
	for (uint i = 0; i < GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players.size(); ++i)
	{
		if (platform->transform->GetGlobalPosition().y > GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players[i]->transform->GetGlobalPosition().y - 3)
		{
			GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players[0]->transform->SetGlobalPosition(platform->transform->GetGlobalPosition() + float3(0, 5, 0));
			GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players[1]->transform->SetGlobalPosition(platform->transform->GetGlobalPosition() + float3(0, 5, 0));
			GameObject::FindWithName("Main Camera")->transform->SetGlobalPosition(GameObject::FindWithName("Main Camera")->GetComponent<CameraMovement>()->CalculateMidPoint() + GameObject::FindWithName("Main Camera")->GetComponent<CameraMovement>()->trg_offset);
			GameObject::FindWithName("GameManager")->GetComponent<PlayerManager>()->players[i]->ReceiveDamage(200);
		}
	}
}