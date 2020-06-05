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

		damagecount_player1->SetAlpha(1);
	}
	else
	{
		damage_num->starting_y_position = damagecount_player2->game_object_attached->transform->GetGlobalPosition().y - 350 * (player2_damagenums.size() + 1);

		damage_num->go = GameObject::Instantiate(text, 
			float3(damagecount_player2->game_object_attached->transform->GetGlobalPosition().x + 75,
			damage_num->starting_y_position,
			0), false, damagecount_player2->game_object_attached);

		damagecount_player2->SetAlpha(1.0f);
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

	if (!player1_damagenums.empty())
	{
		auto iter = player1_damagenums.begin();
		for (; iter != player1_damagenums.end(); ++iter)
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
				float alpha = Maths::Lerp(1.0f, 0.0f, t);

				(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, lerp, 0));
				(*iter)->text->SetAlpha(alpha);

				if (t >= 1)
				{
					(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, 0, 0));
					(*iter)->text->SetAlpha(0.0f);
					damagecount_player1->SetText(std::to_string(std::stoi(damagecount_player1->GetText()) + (*iter)->damage).c_str());
					GameObject::Destroy((*iter)->go);
					(*iter)->go = nullptr;
					player1_damagenums.erase(iter);

					if (player1_damagenums.size() == 0)
					{
						damage_count1_time = internal_timer;
					}

					--iter;
				}
			}
		}
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
				damagecount_player1->SetText("000");
			}
		}
	}


	if (!player2_damagenums.empty())
	{
		auto iter = player2_damagenums.begin();
		for (; iter != player2_damagenums.end(); ++iter)
		{
			if (!(*iter)->is_transitioning && ((*iter)->current_timer + 1 <= internal_timer))
			{
				(*iter)->is_transitioning = true;
				(*iter)->current_timer = internal_timer;
			}

			if ((*iter)->is_transitioning)
			{
				float t = (internal_timer - (*iter)->current_timer) / 0.5f;
				float lerp = Maths::Lerp((*iter)->starting_y_position, game_object->parent->transform->GetGlobalPosition().y, t);
				float alpha = Maths::Lerp(0.0f, 1.0f, t);

				(*iter)->go->transform->SetGlobalPosition(float3((*iter)->go->transform->GetGlobalPosition().x, lerp, 0));
				(*iter)->text->SetAlpha(alpha);

				if (t >= 1)
				{
					(*iter)->go->transform->SetGlobalPosition(game_object->parent->transform->GetGlobalPosition());
					(*iter)->text->SetAlpha(0.0f);
					damagecount_player2->SetText(std::to_string(std::stoi(damagecount_player2->GetText()) + (*iter)->damage).c_str());
					GameObject::Destroy((*iter)->go);
					(*iter)->go = nullptr;
					player2_damagenums.erase(iter);

					if (player2_damagenums.size() == 0)
					{
						damage_count2_time = internal_timer;
					}

					--iter;
				}
			}
		}
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
				damagecount_player2->SetText("000");
			}
		}
	}
}