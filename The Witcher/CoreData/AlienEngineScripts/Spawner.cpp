#include "Spawner.h"

Spawner::Spawner() : Alien()
{
}

Spawner::~Spawner()
{
}

void Spawner::Start()
{
}

void Spawner::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		Spawn(TO_SPAWN::HEAD, game_object->transform->GetGlobalPosition());
	}
}

void Spawner::Spawn(TO_SPAWN type, float3 position)
{
	switch (type)
	{
	case TO_SPAWN::HEAD:
	{
		heads_spawned.push_back(GameObject::Instantiate(enemy_heads, position));
		break;
	}
	case TO_SPAWN::COIN:
	{
		GameObject::Instantiate(coin, position);
		break;
	}
	case TO_SPAWN::RELIC:
	{
		GameObject::Instantiate(relic, position);
		break;
	}
	}
}
