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

	if (spawn)
	{
		ComponentTransform** c_trans = nullptr;
		int size = spawn->GetComponentsInChildren(ComponentType::TRANSFORM, (Component***)&c_trans, false);

		for (uint i = 0u; i < size; ++i) {

			DropRelic(c_trans[i]->GetGlobalPosition());
		}

		GameObject::FreeArrayMemory((void***)&c_trans);
	}
	
}

void RelicManager::Update()
{
}

void RelicManager::DropRelic(float3 position)
{
	int random_index = Random::GetRandomIntBetweenTwo(1, 100);
	if (random_index > 0 && random_index <= 60)
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
	else if (random_index <= 80)
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
	else if (random_index <= 95)
	{
		random_index = Random::GetRandomIntBetweenTwo(1, 4);
		switch (random_index)
		{
		case 1:
			GameObject::Instantiate(nature_soul, position);
			break;
		case 2:
			GameObject::Instantiate(sea_soul, position);
			break;
		case 3:
			GameObject::Instantiate(djinn_jar, position);
			break;
		case 4:
			GameObject::Instantiate(strigas_claws, position);
			break;
		default:
			break;
		}
	}
	else
		GameObject::Instantiate(witcher_rage, position);
}
