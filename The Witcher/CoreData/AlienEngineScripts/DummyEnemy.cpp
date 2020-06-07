#include "DummyEnemy.h"
#include "PlayerAttacks.h"
#include "AttackTrigger.h"
#include "PlayerController.h"

DummyEnemy::DummyEnemy() : Alien()
{
}

DummyEnemy::~DummyEnemy()
{
}


void DummyEnemy::Start()
{
}

void DummyEnemy::Update()
{
	if (showing_combo)
	{
		if (time_showing + 2.f <= Time::GetTimeSinceStart())
		{
			DestroyCombo();
		}
	}
	//perlinNoisePos += 10 * Time::GetDT();
	//float angle = DegToRad(maxAngle);
	
	//game_object->transform->SetLocalRotation(Quat::RotateX(Maths::PerlinNoise(0, perlinNoisePos, 0.8f, 0.8f) * angle - (angle * 0.5f)));
	if (wiggle)
	{
		
	}
}

void DummyEnemy::OnTriggerEnter(ComponentCollider* col)
{
	
	if (strcmp(col->game_object_attached->GetTag(), "PlayerAttack") == 0)
	{ 
		LOG("Entered ");
		wiggle = true;
		Tween::TweenRotate(game_object, float3(0, 0, 0), 5, Tween::easeInElastic);
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

			if (player->attacks->GetCurrentAttack()->info.name.back() == 'L')
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

void DummyEnemy::DestroyCombo()
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