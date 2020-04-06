#include "GameManager.h"

GameManager::GameManager() : Alien()
{
}

GameManager::~GameManager()
{
}

void GameManager::Awake()
{
	Game_Manager = this;
	event_manager = (EventManager*)game_object->GetComponentScript("EventManager");
	enemy_manager = (EnemyManager*)game_object->GetComponentScript("EnemyManager");
	dialogue_manager = (DialogueManager*)game_object->GetComponentScript("DialogueManager");
}

void GameManager::Start()
{
}

void GameManager::Update()
{
}