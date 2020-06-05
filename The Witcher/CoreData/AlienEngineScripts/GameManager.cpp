#include "GameManager.h"
#include "EventManager.h"
#include "EnemyManager.h"
#include "DialogueManager.h"
#include "PlayerManager.h"
#include "RelicManager.h"
#include "ParticlePool.h"
#include "EffectsFactory.h"
#include "RumblerManager.h"

GameManager::GameManager() : Alien()
{

}

GameManager::~GameManager()
{
	delete effects_factory;
}

void GameManager::Awake()
{
	//effects_factory = new EffectsFactory();

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
