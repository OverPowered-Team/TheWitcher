#include "RelicManager.h"

RelicManager::RelicManager() : Alien()
{
}

RelicManager::~RelicManager()
{
}

void RelicManager::Start()
{
	spawn = GameObject::FindWithName("SpawnRelics");

	if (spawn && spawn->GetChildren().size() > 0)
	{
		auto c_trans = spawn->GetComponentsInChildren<ComponentTransform>();

		for (auto i = c_trans.begin(); i != c_trans.end(); ++i) {

			DropRelic((*i)->GetGlobalPosition());
		}
	}
}

void RelicManager::Update()
{
}

void RelicManager::DropRelic(float3 position)
{
	int random_index = Random::GetRandomIntBetweenTwo(1, 100);
	if (random_index > 0 && random_index <= 75)
	{
		random_index = Random::GetRandomIntBetweenTwo(1, 5);
		switch (random_index)
		{
		case 1:
			GameObject::Instantiate(fire_runestone, position);
			break;
		case 2:
			GameObject::Instantiate(ice_runestone, position);
			break;
		case 3:
			GameObject::Instantiate(earth_runestone, position);
			break;
		case 4:
			GameObject::Instantiate(lightning_runestone, position);
			break;
		case 5:
			GameObject::Instantiate(poison_runestone, position);
			break;
		default:
			break;
		}
	}
	else 
	{
		random_index = Random::GetRandomIntBetweenTwo(1, 4);
		switch (random_index)
		{
		case 1:
			GameObject::Instantiate(fire_oil, position);
			break;
		case 2:
			GameObject::Instantiate(ice_oil, position);
			break;
		case 3:
			GameObject::Instantiate(earth_oil, position);
			break;
		case 4:
			GameObject::Instantiate(poison_oil, position);
			break;
		default:
			break;
		}
	}
}
