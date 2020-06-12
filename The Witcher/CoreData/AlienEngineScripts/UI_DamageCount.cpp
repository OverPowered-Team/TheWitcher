#include "UI_DamageCount.h"
#include "PlayerController.h"
#include "Scores_Data.h"
#include "PlayerAttacks.h"
#include "UI_Char_Frame.h"

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
		if (player1_damagenums.empty())
		{
			damage_num->starting_y_position = damagecount_player1->game_object_attached->transform->GetGlobalPosition().y - 195.5f;
		}
		else
		{
			damage_num->starting_y_position = player1_damagenums.back()->starting_y_position - 175;
			if (player->attacks->GetCurrentAttack()->info.name.size() == 1 && player1_damagenums.size() > 1 && player1_damagenums.back()->is_last == false)
			{
				auto iter = player1_damagenums.begin();
				for (; iter != player1_damagenums.end(); ++iter)
				{
					(*iter)->current_timer = internal_timer;
					transition_player1_damagenums.push_back((*iter));
					player1_damagenums.erase(iter);
					--iter;
				}
			}
		}

		damage_num->go = GameObject::Instantiate(text_left,
			float3(damagecount_player1->game_object_attached->transform->GetGlobalPosition().x + 75,
				damage_num->starting_y_position,
				0), false, GameObject::FindWithName("List_DmgNums1"));

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
		if (player2_damagenums.empty())
		{
			damage_num->starting_y_position = damagecount_player2->game_object_attached->transform->GetGlobalPosition().y - 195.5f;
		}
		else
		{
			damage_num->starting_y_position = player2_damagenums.back()->starting_y_position - 175;
			if (player->attacks->GetCurrentAttack()->info.name.size() == 1 && player2_damagenums.size() > 1 && player2_damagenums.back()->is_last == false)
			{
				auto iter = player2_damagenums.begin();
				for (; iter != player2_damagenums.end(); ++iter)
				{
					(*iter)->current_timer = internal_timer;
					transition_player2_damagenums.push_back((*iter));
					player2_damagenums.erase(iter);
					--iter;
				}
			}
		}

		damage_num->go = GameObject::Instantiate(text_right, 
			float3(damagecount_player2->game_object_attached->transform->GetGlobalPosition().x+100,
			damage_num->starting_y_position,
			0), false, GameObject::FindWithName("List_DmgNums2"));

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
	
	if (player->attacks->GetCurrentAttack()->IsLast())
	{
		damage_num->is_last = true;
		damage_num->combo_text = damage_num->go->GetChild("Combo")->GetComponent<ComponentText>();
		damage_num->combo_text->SetAlpha(1);

		int sign = 0;

		if (player->controller_index == 1)
		{
			sign = 1;
		}
		else
		{
			sign = -1;
		}

		// Checking for relics 
		if (player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Fire))
		{
			damage_num->relic_images.push_back(damage_num->go->GetChild("Fire")->GetComponent<ComponentImage>());
		}

		if (player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Ice))
		{
			damage_num->relic_images.push_back(damage_num->go->GetChild("Ice")->GetComponent<ComponentImage>());
			damage_num->relic_images.back()->game_object_attached->transform->SetLocalPosition(
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().x + sign * 250 * (damage_num->relic_images.size() - 1),
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().y,
				0);
		}

		if (player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Poison))
		{
			damage_num->relic_images.push_back(damage_num->go->GetChild("Poison")->GetComponent<ComponentImage>());
			damage_num->relic_images.back()->game_object_attached->transform->SetLocalPosition(
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().x + sign * 250 * (damage_num->relic_images.size() - 1),
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().y,
				0);
		}

		if (player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Earth))
		{
			damage_num->relic_images.push_back(damage_num->go->GetChild("Earth")->GetComponent<ComponentImage>());
			damage_num->relic_images.back()->game_object_attached->transform->SetLocalPosition(
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().x + sign * 250 * (damage_num->relic_images.size() - 1),
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().y,
				0);
		}

		if (player->attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Lightning))
		{
			damage_num->relic_images.push_back(damage_num->go->GetChild("Lighting")->GetComponent<ComponentImage>());
			damage_num->relic_images.back()->game_object_attached->transform->SetLocalPosition(
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().x + sign * 250 * (damage_num->relic_images.size() - 1),
				damage_num->relic_images.back()->game_object_attached->transform->GetLocalPosition().y,
				0);
		}

		auto iter = damage_num->relic_images.begin();
		for (; iter != damage_num->relic_images.end(); ++iter)
		{
			(*iter)->current_color.a = 1;
		}
	}

	damage_num->damage = damage;
	damage_num->go->transform->SetLocalScale(float3(2.0f, 2, 1));
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

void UI_DamageCount::PlayerHasBeenHit(PlayerController* player)
{
	ComponentText* text = nullptr;
	std::vector<DamageNum*>* vector_to_handle = nullptr;
	std::vector<DamageNum*>* vector_to_transition = nullptr;

	if (player->controller_index == 1)
	{
		text = damagecount_player1;
		vector_to_handle = &player1_damagenums;
		vector_to_transition = &transition_player1_damagenums;
	}
	else
	{
		text = damagecount_player2;
		vector_to_handle = &player2_damagenums;
		vector_to_transition = &transition_player2_damagenums;
	}

	auto iter = (*vector_to_handle).begin();
	for (; iter != (*vector_to_handle).end(); ++iter)
	{
		(*iter)->current_timer = internal_timer;
		(*vector_to_transition).push_back((*iter));
		(*vector_to_handle).erase(iter);
		--iter;
	}
}

void UI_DamageCount::AddRemainingComboPoints()
{
	Scores_Data::player1_damage += stoi(damagecount_player1->GetText());
	Scores_Data::player2_damage += stoi(damagecount_player2->GetText());
}

void UI_DamageCount::UpdateTimes(float time_paused)
{
	internal_timer += time_paused;
	fadein_timer1 += time_paused;
	fadein_timer2 += time_paused;
	damage_count1_time += time_paused;
	damage_count2_time += time_paused;
	scaling_time1 += time_paused;
	scaling_time2 += time_paused;
	start_shake_time1 += time_paused;
	start_shake_time2 += time_paused;

	for (auto i = player1_damagenums.begin(); i != player1_damagenums.end(); ++i)
	{
		(*i)->current_timer += time_paused;
	}
	for (auto i = player2_damagenums.begin(); i != player2_damagenums.end(); ++i)
	{
		(*i)->current_timer += time_paused;
	}
	for (auto i = transition_player1_damagenums.begin(); i != transition_player1_damagenums.end(); ++i)
	{
		(*i)->current_timer += time_paused;
	}
	for (auto i = transition_player2_damagenums.begin(); i != transition_player2_damagenums.end(); ++i)
	{
		(*i)->current_timer += time_paused;
	}
}

void UI_DamageCount::Start()
{
	damagecount_player1 = game_object->GetChild("Parent_DamageCount_Player1")->GetChild("DamageCount_Player1")->GetComponent<ComponentText>();
	damagecount_player2 = game_object->GetChild("Parent_DamageCount_Player2")->GetChild("DamageCount_Player2")->GetComponent<ComponentText>();
	damagecount_player1->SetAlpha(0);
	damagecount_player2->SetAlpha(0);

	original_scale = game_object->GetChild("Parent_DamageCount_Player1")->transform->GetLocalScale().x;

	original_position_x1= game_object->GetChild("Parent_DamageCount_Player1")->transform->GetLocalPosition().x;
	original_position_y1= game_object->GetChild("Parent_DamageCount_Player1")->transform->GetLocalPosition().y;
	original_position_x2= game_object->GetChild("Parent_DamageCount_Player2")->transform->GetLocalPosition().x;
	original_position_y2= game_object->GetChild("Parent_DamageCount_Player2")->transform->GetLocalPosition().y;

}

void UI_DamageCount::Update()
{

	internal_timer += Time::GetDT();

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

	if (!player1_damagenums.empty() || !transition_player1_damagenums.empty())
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
				shake_goal1 = 100;
				damagecount_player1->SetAlpha(0);
				Scores_Data::player1_damage += stoi(damagecount_player1->GetText());
				damagecount_player1->SetText("0");
				game_object->GetChild("Character1")->GetComponent<UI_Char_Frame>()->PlayerOnFire(false);
			}
		}
	}


	if (!player2_damagenums.empty() || !transition_player2_damagenums.empty())
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
				shake_goal2 = 100;
				damagecount_player2->SetAlpha(0);
				Scores_Data::player2_damage += stoi(damagecount_player2->GetText());
				damagecount_player2->SetText("0");
				game_object->GetChild("Character2")->GetComponent<UI_Char_Frame>()->PlayerOnFire(false);
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

	if (is_shaking1)
	{
		Shake(1);
	}

	if (is_shaking2)
	{
		Shake(2);
	}
}

void UI_DamageCount::DamageCount_Handling(int index)
{
	ComponentText* text = nullptr;
	std::vector<DamageNum*>* vector_to_handle = nullptr;
	std::vector<DamageNum*>* vector_to_transition = nullptr;

	if (index == 1)
	{
		text = damagecount_player1;
		vector_to_handle = &player1_damagenums;
		vector_to_transition = &transition_player1_damagenums;
	}
	else
	{
		text = damagecount_player2;
		vector_to_handle = &player2_damagenums;
		vector_to_transition = &transition_player2_damagenums;
	}

	if (!(*vector_to_handle).empty())
	{
		// Checking if enough time has passed to quit combo counting
		if ((*vector_to_handle).back()->current_timer + 3 <= internal_timer)
		{
			auto iter = (*vector_to_handle).begin();
			for (; iter != (*vector_to_handle).end(); ++iter)
			{
				(*iter)->current_timer = internal_timer;
				(*vector_to_transition).push_back((*iter));
				(*vector_to_handle).erase(iter);
				--iter;
			}
		}

		auto iter = (*vector_to_handle).begin();
		for (; iter != (*vector_to_handle).end(); ++iter)
		{
			if ((*iter) == (*vector_to_handle).front() && (*iter)->go->transform->GetLocalPosition().y != -175)
			{
				auto iter = (*vector_to_handle).begin();
				for (; iter != (*vector_to_handle).end(); ++iter)
				{
					float t = (internal_timer - (*iter)->current_timer) / 0.25f;
					float lerp = 0.0f;

					if ((*iter) == (*vector_to_handle).front())
					{
						lerp = Maths::Lerp((*iter)->starting_y_position, -175.f, t);
					}
					else
					{
						lerp = Maths::Lerp((*iter)->starting_y_position, (*(iter - 1))->go->transform->GetLocalPosition().y - 175.f, t);
					}

					(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, lerp, 0));

					if (t >= 1)
					{
						if ((*iter) == (*vector_to_handle).front())
						{
							(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, -175, 0));
						}
						else
						{
							(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, -(*(iter - 1))->go->transform->GetLocalPosition().y - 175.f, 0));
						}
						(*iter)->starting_y_position = (*iter)->go->transform->GetLocalPosition().y;

					}
				}
			}

			// New DamageNum Effect
			if ((*iter)->current_timer + 0.25f >= internal_timer)
			{
				float t = (internal_timer - (*iter)->current_timer) / 0.1f;
				float lerp = Maths::Lerp(2.0f, 0.5f, t);

				(*iter)->go->transform->SetLocalScale(lerp, lerp, 1);

				if (t >= 1)
				{
					(*iter)->go->transform->SetLocalScale(0.5f, 0.5f, 1);
				}
			}

			if ((*iter)->is_last && (*iter)->current_timer + 1.5f <= internal_timer)
			{
				auto iter_ = std::make_reverse_iterator(iter + 1);
				for (; iter_ != (*vector_to_handle).rend(); ++iter_)
				{
					(*iter_)->current_timer = internal_timer;
					(*vector_to_transition).push_back((*iter_));
					(*vector_to_handle).erase((iter_ + 1).base());
				}
				iter = ((*vector_to_handle).begin() - 1);
			}
		}
	}

	if (!(*vector_to_transition).empty())
	{
		auto iter = (*vector_to_transition).begin();
		for (; iter != (*vector_to_transition).end(); ++iter)
		{
			float t = (internal_timer - (*iter)->current_timer) / 0.25f;
			float lerp = Maths::Lerp((*iter)->starting_y_position, 0.0f, t);
			float alpha = Maths::Lerp(1.0f, 0.0f, t);

			(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, lerp, 0));
			(*iter)->text->SetAlpha(alpha);

			if ((*iter)->is_last)
			{
				(*iter)->combo_text->current_color.a = alpha;
				if (!(*iter)->relic_images.empty())
				{
					auto image = (*iter)->relic_images.begin();
					for (; image != (*iter)->relic_images.end(); ++image)
					{
						(*image)->current_color.a = alpha;
					}
				}
			}

			if (t >= 1)
			{
				(*iter)->go->transform->SetLocalPosition(float3((*iter)->go->transform->GetLocalPosition().x, 0, 0));
				(*iter)->text->SetAlpha(0.0f);
				text->SetText(std::to_string(std::stoi(text->GetText()) + (*iter)->damage).c_str());
				GameObject::Destroy((*iter)->go);
				(*iter)->go = nullptr;
				(*vector_to_transition).erase(iter);

				if (index == 1)
				{
					if (shake_goal1 <= stoi(text->GetText()))
					{
						game_object->GetChild("Character1")->GetComponent<UI_Char_Frame>()->PlayerOnFire(true);
						shake_goal1 += 50;
						is_shaking1 = true;
						start_shake_time1 = internal_timer;
					}
				}
				else
				{
					if (shake_goal2 <= stoi(text->GetText()))
					{
						game_object->GetChild("Character2")->GetComponent<UI_Char_Frame>()->PlayerOnFire(true);
						shake_goal2 += 50;
						is_shaking2 = true;
						start_shake_time2 = internal_timer;
					}
				}


				if ((*vector_to_transition).size() == 0)
				{
					if (index == 1)
					{
						is_scaling1 = true;
						
						damage_count1_time = internal_timer;
						scaling_time1 = internal_timer;
					}
					else
					{
						is_scaling2 = true;
						damage_count2_time = internal_timer;
						scaling_time2 = internal_timer;
					}
				}

				--iter;
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
		damage_count = damagecount_player1->game_object_attached->parent;
		time = scaling_time1;
	}
	else
	{
		damage_count = damagecount_player2->game_object_attached->parent;
		time = scaling_time2;
	}

	float t = (internal_timer - time) / 0.25f;
	float lerp = 0.0f;

	if (t <= 0.25f)
	{
		lerp = Maths::Lerp(original_scale, original_scale * (1.f + 1.f / 4.f), t / 0.25f);
	}
	else
	{
		lerp = Maths::Lerp(original_scale * (1.f + 1.f / 4.f), original_scale, (t - 0.25f) / (4 / 3));
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

void UI_DamageCount::Shake(int index)
{
	GameObject* damage_count = nullptr;
	float original_position_x = 0.0f;
	float original_position_y = 0.0f;
	float time = 0.0f;

	if (index == 1)
	{
		damage_count = damagecount_player1->game_object_attached->parent;
		original_position_x = original_position_x1;
		original_position_y = original_position_y1;
		time = start_shake_time1;
	}
	else
	{
		damage_count = damagecount_player2->game_object_attached->parent;
		original_position_x = original_position_x2;
		original_position_y = original_position_y2;
		time = start_shake_time2;
	}

	post_off_set += off_set * Time::GetDT();

	damage_count->transform->SetLocalPosition(original_position_x + ((Maths::PerlinNoise(0, post_off_set, 0.8f, 0.8f) * 4) - 2),
		original_position_y + ((Maths::PerlinNoise(1, 0.8f, post_off_set, 0.8f) * 4) - 2),
		0.f);

	if ((internal_timer - time) > 0.5f)
	{
		post_off_set = off_set;
		damage_count->transform->SetLocalPosition(original_position_x, original_position_y, 0.f);

		if (index == 1)
		{
			is_shaking1 = false;
		}
		else
		{
			is_shaking2 = false;
		}
	}
}
