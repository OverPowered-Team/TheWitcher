#include "Spawner.h"

Spawner::Spawner() : Alien()
{
}

Spawner::~Spawner()
{
}

void Spawner::Spawn(TO_SPAWN type, float3 position, int head_type)
{
	Prefab head;

	switch (head_type)
	{
	case 0:
	{
		head = ghoul_head;
		break;
	}
	case 1:
	{
		head = nilfgaardian_head;
		break;
	}
	case 2:
	{
		head = leshen_head;
		break;
	}
	case 3:
	{
		head = ciri_head;
		break;
	}
	case 4:
	{
		return;
	}
	case 5:
	{
		head = drowned_head;
		break;
	}
	}

	switch (type)
	{
	case TO_SPAWN::HEAD:
	{
		GameObject::Instantiate(head, position);
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
