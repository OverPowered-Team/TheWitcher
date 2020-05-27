#include "Dummie.h"
#include "PlayerAttacks.h"
#include "AttackTrigger.h"
#include "PlayerController.h"

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
	if (showing_combo)
	{
		if (time_showing + 2.f <= Time::GetTimeSinceStart())
		{
			DestroyCombo();
		}
	}
}

void Dummie::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "PlayerAttack") == 0)
	{
		if (player == nullptr)
		{
			player = col->game_object_attached->GetComponent<AttackTrigger>()->player;
		}

		if (col->game_object_attached->GetComponent<AttackTrigger>()->player == player)
		{
			if (current_buttons.size() >= 5)
			{
				DestroyCombo();
			}

			float position = current_buttons.size() * 5.f - 10.f;

			if(player->attacks->GetCurrentAttack()->info.name.back() == 'L')
			{
				current_buttons.push_back(GameObject::Instantiate(button_x, float3(position, 0, 0), false, game_object->GetChild("Combo_UI")));
			}
			else
			{
				current_buttons.push_back(GameObject::Instantiate(button_y, float3(position, 0, 0), false, game_object->GetChild("Combo_UI")));

			}

			showing_combo = true;
			time_showing = Time::GetTimeSinceStart();
		}
	}
}

void Dummie::DestroyCombo()
{
	auto iter = current_buttons.begin();
	for (; iter != current_buttons.end(); ++iter)
	{
		(*iter)->Destroy((*iter));
	}
	current_buttons.clear();
	showing_combo = false;
	player = nullptr;
}
