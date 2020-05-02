#include "Dummie.h"

Dummie::Dummie() : Alien()
{
}

Dummie::~Dummie()
{
}

void Dummie::Start()
{
}

void Dummie::Update()
{
}

void Dummie::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "PlayerAttack") == 0)
	{
		LOG(col->game_object_attached->GetName());
	}
}
