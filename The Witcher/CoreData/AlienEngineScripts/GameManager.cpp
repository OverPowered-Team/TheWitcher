#include "GameManager.h"
#include "EventManager.h"
#include "EnemyManager.h"
#include "DialogueManager.h"
#include "PlayerManager.h"
#include "RelicManager.h"
#include "RumblerManager.h"

GameManager::GameManager() : Alien()
{
	manager = this;
}

GameManager::~GameManager()
{
}

void GameManager::Awake()
{
	event_manager =	game_object->GetComponent<EventManager>();
	enemy_manager =	game_object->GetComponent<EnemyManager>();
	dialogue_manager = game_object->GetComponent<DialogueManager>();
	player_manager = game_object->GetComponent<PlayerManager>();
	relic_manager =	game_object->GetComponent<RelicManager>();
	rumbler_manager = game_object->GetComponent<RumblerManager>();
}

void GameManager::Start()
{
}

void GameManager::Update()
{
	
}
