#include "UI_DamageCount.h"
#include "PlayerController.h"
#include "Scores_Data.h"

UI_DamageCount::UI_DamageCount() : Alien()
{
}

UI_DamageCount::~UI_DamageCount()
{
}

void UI_DamageCount::AddDamageCount(float damage, PlayerController* player)
{
	DamageNum* damage_num = new DamageNum();
	
	if (player->controller_index == 1)
	{
		damage_num->starting_y_position = damagecount_player1->game_object_attached->transform->GetGlobalPosition().y - 350 * (player1_damagenums.size() + 1);

		damage_num->go = GameObject::Instantiate(text,
			float3(damagecount_player1->game_object_attached->transform->GetGlobalPosition().x + 75,
				damage_num->starting_y_position,
				0), false, damagecount_player1->game_object_attached);

		if (!is_fading_in1 && damagecount_player1->current_color.a == 0)
		{
			is_fading_in1 = true;
			fadein_timer1 = internal_timer;
		}
		else
		{
			damagecount_player1->SetAlpha(1);
		}
	}
	else
	{
		damage_num->starting_y_position = damagecount_player2->game_object_attached->transform->GetGlobalPosition().y - 350 * (player2_damagenums.size() + 1);

		damage_num->go = GameObject::Instantiate(text, 
			float3(damagecount_player2->game_object_attached->transform->GetGlobalPosition().x + 75,
			damage_num->starting_y_position,
			0), false, damagecount_player2->game_object_attached);

		if (!is_fading_in2 && damagecount_player2->current_color.a == 0)
		{
			is_fading_in2 = true;
			fadein_timer2 = internal_timer;
		}
		else
		{
			damagecount_player2->SetAlpha(1.0f);
		}
	}
		
	damage_num->damage = damage;
	damage_num->text = damage_num->go->GetComponent<ComponentText>();
	damage_num->text->SetText(std::to_string((int)damage).c_str());
	damage_num->text->SetColor(float3(1, 1, 1));
	damage_num->text->SetAlpha(1);
	damage_num->current_timer = internal_timer;

	if (player->controller_index == 1)
	{
		player1_damagenums.push_back(damage_num);
	}
	else
	{
		player2_damagenums.push_back(damage_num);
	}
}

void UI_DamageCount::Start()
{
	damagecount_player1 = game_object->GetChild("DamageCount_Player1")->GetComponent<ComponentText>();
	damagecount_player2 = game_object->GetChild("DamageCount_Player2")->GetComponent<ComponentText>();
	damagecount_player1->SetAlpha(0);
	damagecount_player2->SetAlpha(0);

	original_scale = game_object->GetChild("DamageCount_Player1")->transform->GetLocalScale().x;
}

void UI_DamageCount::Update()
{
	if (!Time::IsGamePaused())
	{
		internal_timer += Time::GetGameTime() - (internal_timer + time_paused);

		if (time_paused != 0.0f)
		{
			time_paused = 0.0f;
		}
	}
	else
	{
		time_paused = Time::GetGameTime() - internal_timer;
		return;
	}

	if (is_fading_in1)
	{
		float t = internal_timer - fadein_timer1;
		float lerp = Maths::Lerp(0.0f, 1.0f, t);

		damagecount_player1->SetAlpha(lerp);

		if (t >= 1)
		{
			damagecount_player1->SetAlpha(1);
			is_fading_in1 = false;
		}
	}

	if (is_fading_in2)
	{
		float t = internal_timer - fadein_timer2;
		float lerp = Maths::Lerp(0.0f, 1.0f, t);

		damagecount_player2->SetAlpha(lerp);

		if (t >= 1)
		{
			damagecount_player2->SetAlpha(1);
			is_fading_in2 = false;
		}
	}

	if (!player1_damagenums.empty())
	{
		DamageCount_Handling(1);
	}
	else
	{
		if (damagecount_player1->current_color.a != 0 && (damage_count1_time + 2 <= internal_timer))
		{
			float t = (internal_timer - (damage_count1_time + 2)) / 5;
			float lerp = Maths::Lerp(1.0f, 0.0f, t);

			damagecount_player1->SetAlpha(lerp);

			if (t >= 1)
			{
				damagecount_player1->SetAlpha(0);
				Scores_Data::player1_damage += stoi(damagecount_player1->GetText());
				damagecount_player1->SetText("00");
			}
		}
	}


	if (!player2_damagenums.empty())
	{
		DamageCount_Handling(2);
	}
	else
	{
		if (damagecount_player2->current_color.a != 0 && (damage_count2_time + 2 <= internal_timer))
		{
			float t = (internal_timer - (damage_count2_time + 2)) / 5;
			float lerp = Maths::Lerp(1.0f, 0.0f, t);

			damagecount_player2->SetAlpha(lerp);

			if (t >= 1)
			{
				damagecount_player2->SetAlpha(0);
				Scores_Data::player2_damage += stoi(damagecount_player2->GetText());
				damagecount_player2->SetText("00");
			}
		}
	}

	if (is_scaling1)
	{
		ScaleDamageCount(1);
	}

	if (is_scaling2)
	{
		ScaleDamageCount(2);
	}
}

void UI_DamageCount::DamageCount_Handling(int index)
{
	ComponentText* text = nullptr;
	std::vector<DamageNum*>* vector_to_handle = nullptr;

	if (index == 1)
	{
		text = damagecount_player1;
		vector_to_handle = &player1_damagenums;
	}
	else
	{
		text = damagecount_player2;
		vector_to_handle = &player2_damagenums;
	}

	auto iter = (*vector_to_handle).begin();
	for (; iter != (*vector_to_handle).end(); ++iter)
	{
		if (!(*iter)->is_transitioning && ((*iter)->current_timer + 1 <= internal_timer))
		{
			(*iter)->is_transitioning = true;
			(*iter)->current_timer = internal_timer;
		}

		if ((*iter)->is_transitioning)
		{
			float t = (internal_timer - (*iter)->current_timer) / 0.5f;
			float lerp = Maths::Lerp((*iter)->starting_y_position, 0.0f, t);
			float alpha = Maths::Lerp(0.0f, 1.0f, t);

			(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, lerp, 0));
			(*iter)->text->SetAlpha(alpha);

			if (t >= 1)
			{
				(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, 0, 0));
				(*iter)->text->SetAlpha(0.0f);
				text->SetText(std::to_string(std::stoi(text->GetText()) + (*iter)->damage).c_str());
				GameObject::Destroy((*iter)->go);
				(*iter)->go = nullptr;
				(*vector_to_handle).erase(iter);

				if ((*vector_to_handle).size() == 0)
				{
					if (index == 1)
					{
						damage_count1_time = internal_timer;
						scaling_time1 = internal_timer;
					}
					else
					{
						damage_count2_time = internal_timer;
						scaling_time2 = internal_timer;
					}
				}

				--iter;

				if (index == 1)
				{
					is_scaling1 = true;
					scaling_time1 = internal_timer;
				}
				else
				{
					is_scaling2 = true;
					scaling_time2 = internal_timer;
				}
			}
		}
	}
}

void UI_DamageCount::ScaleDamageCount(int index)
{
	GameObject* damage_count = nullptr;
	float time = 0.0f;

	if (index == 1)
	{
		damage_count = damagecount_player1->game_object_attached;
		time = scaling_time1;
	}
	else
	{
		damage_count = damagecount_player2->game_object_attached;
		time = scaling_time2;
	}

	float t = (internal_timer - time) / 0.5f;
	float lerp = 0.0f;

	if (t <= 0.5f)
	{
		lerp = Maths::Lerp(original_scale, original_scale * (1.f + 1.f / 4.f), t / 0.5f);
	}
	else
	{
		lerp = Maths::Lerp(original_scale * (1.f + 1.f / 4.f), original_scale, (t - 0.5f) / 0.5f);
	}

	damage_count->transform->SetLocalScale(float3(lerp, lerp, 1));

	if (t >= 1)
	{
		damage_count->transform->SetLocalScale(float3(original_scale, original_scale, 1));
		if (index == 1)
		{
			is_scaling1 = false;
		}
		else
		{
			is_scaling2 = false;
		}
	}
}
