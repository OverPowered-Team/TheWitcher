#include "Credits.h"

Credits::Credits() : Alien()
{
}

Credits::~Credits()
{
}

void Credits::Start()
{
}

void Credits::Update()
{
	if (transform->GetLocalPosition().y <= last_position)
	{
		float position_to_add = (last_position / time_credits_last) * Time::GetDT();
		transform->AddPosition(float3(0, position_to_add, 0));
		if (transform->GetLocalPosition().y >= last_position)
		{
			time = Time::GetGameTime();
		}
	}
	else
	{
		if (Time::GetGameTime() - time >= 2.f)
		{
			SceneManager::LoadScene("Main_Menu", FadeToBlackType::VERTICAL_CURTAIN);
		}
	}
}