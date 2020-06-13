#include "GameManager.h"
#include "EventManager.h"
#include "EnemyManager.h"
#include "DialogueManager.h"
#include "PlayerManager.h"
#include "RelicManager.h"
#include "ParticlePool.h"
#include "EffectsFactory.h"
#include "RumblerManager.h"
#include "Scores_Data.h"
#include "UI_DamageCount.h"
#include "PlayerController.h"

GameManager::GameManager() : Alien()
{

}

GameManager::~GameManager()
{
	delete effects_factory;
}

void GameManager::Awake()
{
	effects_factory = new EffectsFactory();

	event_manager = game_object->GetComponent<EventManager>();
	enemy_manager = game_object->GetComponent<EnemyManager>();
	dialogue_manager = game_object->GetComponent<DialogueManager>();
	player_manager = game_object->GetComponent<PlayerManager>();
	relic_manager = game_object->GetComponent<RelicManager>();
	rumbler_manager = game_object->GetComponent<RumblerManager>();
	particle_pool = game_object->GetComponent<ParticlePool>();
}

void GameManager::Start()
{

}

void GameManager::Update()
{
	
}

void GameManager::PrepareDataNextScene(bool dead)
{
	Scores_Data::dead = dead;

	// TO DO: Add maps instead of equaling them
	auto iter = player_manager->players[0]->player_data.type_kills.begin();
	for (; iter != player_manager->players[0]->player_data.type_kills.end(); ++iter)
	{
		Scores_Data::player1_kills[(*iter).first] += (*iter).second;
	}

	iter = player_manager->players[1]->player_data.type_kills.begin();
	for (; iter != player_manager->players[1]->player_data.type_kills.end(); ++iter)
	{
		Scores_Data::player2_kills[(*iter).first] += (*iter).second;
	}

	//Scores_Data::player1_kills = player_manager->players[0]->player_data.type_kills;
	//Scores_Data::player2_kills = player_manager->players[1]->player_data.type_kills;

	GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>()->AddRemainingComboPoints();

	if (!dead)
	{
		Scores_Data::player1_relics = player_manager->players[0]->relics;
		Scores_Data::player2_relics = player_manager->players[1]->relics;
	}

	Scores_Data::last_scene = SceneManager::GetCurrentScene();
}
