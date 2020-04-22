#include "Spawner.h"

Spawner::Spawner() : Alien()
{
}

Spawner::~Spawner()
{
}

void Spawner::Spawn(TO_SPAWN type, float3 position)
{
	switch (type)
	{
	case TO_SPAWN::HEAD:
	{
		GameObject::Instantiate(enemy_heads, position);
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
