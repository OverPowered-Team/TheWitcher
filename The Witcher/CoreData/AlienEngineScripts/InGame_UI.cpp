#include "InGame_UI.h"
#include "PlayerController.h"
#include "UI_Char_Frame.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "UltiBar.h"
#include "Scores_Data.h"
#include "UI_DamageCount.h"
#include "DialogueManager.h"
#include "Effect.h"
#include "PlayerAttacks.h"

InGame_UI::InGame_UI() : Alien()
{
}

InGame_UI::~InGame_UI()
{
}

void InGame_UI::Start()
{
	pause_menu = game_object->GetChild("Pause_Menu");
	pause_menu->SetEnable(false);
	in_game = game_object->GetChild("InGame");
	in_game->SetEnable(true);
	ulti_filter = in_game->GetChild("Ulti_Filter")->GetComponent<ComponentImage>();
	ulti_filter->SetBackgroundColor(0, 0.5f, 1.f, 0.f);

	relics_menu = game_object->GetChild("Relics_Menu");
	relics_menu->SetEnable(false);

	GameObject::FindWithName("Menu")->SetEnable(true);
	canvas = game_object->GetComponent<ComponentCanvas>();
	you_died = GameObject::FindWithName("YouDied");
	relics_panel = GameObject::FindWithName("Relics_Notification");
	relics_panel->SetEnable(false);
	you_died->SetEnable(false);
	
	ulti_bar = game_object->GetChild("InGame")->GetChild("Ulti_Bar")->GetComponent<UltiBar>();
	checkpoint_saved_text = in_game->GetChild("NewCheckpoint");
	component_checkpoint_saved_text = checkpoint_saved_text->GetComponent<ComponentText>();
	checkpoint_saved_text->SetEnable(false);

	for (int i = 0; i < 5; ++i)
	{
		std::vector<ComponentImage*> relics;

		for (int j = 0; j < 5; ++j)
		{
			relics.push_back(relics_menu->GetChild("Relics_G")->GetChild(i)->GetChild(j)->GetComponent<ComponentImage>());
		}

		geralt_relics.push_back(relics);
	}

	for (int i = 0; i < 5; ++i)
	{
		std::vector<ComponentImage*> relics;

		for (int j = 0; j < 5; ++j)
		{
			relics.push_back(relics_menu->GetChild("Relics_Y")->GetChild(i)->GetChild(j)->GetComponent<ComponentImage>());
		}

		yennefer_relics.push_back(relics);
	}
}

void InGame_UI::Update()
{
	if (!Time::IsGamePaused())
	{
		internal_timer += Time::GetGameTime() - (internal_timer + time_paused);

		if (time_paused != 0.0f)
		{
			time_checkpoint += time_paused;
			time += time_paused;
			time_ulti_filter += time_paused;

			for (auto i = particles.begin(); i != particles.end(); ++i)
			{
				(*i)->time_passed += time_paused;
			}

			in_game->GetChild("Character2")->GetComponent<UI_Char_Frame>()->UpdateTimes(time_paused);
			in_game->GetChild("Character1")->GetComponent<UI_Char_Frame>()->UpdateTimes(time_paused);
			in_game->GetChild("Ulti_Bar")->GetComponent<UltiBar>()->UpdateTimes(time_paused);
			in_game->GetComponent<UI_DamageCount>()->UpdateTimes(time_paused);

			time_paused = 0.0f;
		}
	}
	else
	{
		time_paused = Time::GetGameTime() - internal_timer;
	}

	if (((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_START))
		|| (Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_START)) || (Input::GetKeyDown(SDL_SCANCODE_ESCAPE))) 
		&& !died && !relics_menu->IsEnabled())
	{
		PauseMenu(!Time::IsGamePaused());
	}

	if (((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_BACK))
		|| (Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_BACK)))
		&& !died && !pause_menu->IsEnabled())
	{
		RelicsMenu(!Time::IsGamePaused());
	}

	if (relics_menu->IsEnabled())
	{
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B)
			|| (Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B)))
		{
			RelicsMenu(!Time::IsGamePaused());
		}
	}

	if (Time::IsGamePaused())
	{
		return;
	}

	if (checkpoint_saved_text->IsEnabled())
	{
		float t = (internal_timer - time_checkpoint) / 0.5f;
		float lerp = 0.0f;

		switch (checkpoint_state)
		{
		case CP_STATE::FADE_IN:
		{
			lerp = Maths::Lerp(0.f, 1.f, t);
			break;
		}
		case CP_STATE::SHOW:
		{
			lerp = 1;
			break;
		}
		case CP_STATE::FADE_OUT:
		{
			lerp = Maths::Lerp(1.f, 0.f, t);
			break;
		}
		}

		component_checkpoint_saved_text->SetAlpha(lerp);

		if (t >= 1)
		{
			switch (checkpoint_state)
			{
			case CP_STATE::FADE_IN:
			{
				checkpoint_state = CP_STATE::SHOW;
				time_checkpoint = internal_timer + 1.5f;
				break;
			}
			case CP_STATE::SHOW:
			{
				time_checkpoint = internal_timer;
				checkpoint_state = CP_STATE::FADE_OUT;
				break;
			}
			case CP_STATE::FADE_OUT:
			{
				checkpoint_saved_text->SetEnable(false);
				break;
			}
			}
		}
	}

	if (died)
	{
		if (time + waiting < internal_timer)
		{
			if (!died_gone)
			{
				you_died->SetEnable(true);
				time = internal_timer;
				waiting = 2;
				died_gone = true;
			}
			else
			{
				if (strcmp(SceneManager::GetCurrentScene(), "Lvl_1_Tutorial") == 0)
				{
					SceneManager::LoadScene("ForceLoadTutorial", FadeToBlackType::FADE);
				}
				else
				{
					GameManager::instance->PrepareDataNextScene(true);
					SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::VERTICAL_CURTAIN);
				}
			}
		}
	}

	if (!particles.empty())
	{
		auto particle = particles.begin();
		for (; particle != particles.end(); ++particle)
		{
			float lerp = (internal_timer - (*particle)->time_passed) / time_lerp_ult_part;
			float position_x = Maths::Lerp((*particle)->origin_position.x, (*particle)->final_position.x, lerp);
			float position_y = Maths::Lerp((*particle)->origin_position.y, (*particle)->final_position.y, lerp);

			(*particle)->particle->transform->SetLocalPosition(position_x, position_y, 0);

			if (lerp >= 1)
			{
				float new_value = (float)GameManager::instance->player_manager->collective_ultimate_charge / 
					(float)GameManager::instance->player_manager->max_ultimate_charge;

				ulti_bar->UpdateBar(new_value);
				
				GameObject::Destroy((*particle)->particle);
				(*particle) = nullptr;
				particles.erase(particle);
				--particle;
			}
		}
	}

	if (changing_alpha_filter)
	{
		float t = (Time::GetTimeSinceStart() - time_ulti_filter) / 0.25f;
		float lerp = 0.0f;

		if (ulti_active)
		{
			lerp = Maths::Lerp(0.0f, 0.2f, t);
		}
		else
		{
			lerp = Maths::Lerp(0.2f, 0.0f, t);
		}

		ulti_filter->SetBackgroundColor(0, 0.5f, 1.f, lerp);

		if (t >= 1)
		{
			if (ulti_active)
			{
				ulti_filter->SetBackgroundColor(0, 0.5f, 1.f, 0.2f);
			}
			else
			{
				ulti_filter->SetBackgroundColor(0, 0.5f, 1.f, 0.f);
			}

			changing_alpha_filter = false;
		}
	}
}

void InGame_UI::PauseMenu(bool to_open)
{
	in_game->SetEnable(!to_open);
	Time::SetPause(to_open);
	pause_menu->SetEnable(to_open);
	game_object->GetComponent<DialogueManager>()->Pause(to_open);
}

void InGame_UI::RelicsMenu(bool to_open)
{
	in_game->SetEnable(!to_open);
	Time::SetPause(to_open);
	relics_menu->SetEnable(to_open);
	game_object->GetComponent<DialogueManager>()->Pause(to_open);
	if (to_open)
	{
		LoadActiveRelics();
	}
}

void InGame_UI::YouDied()
{
	died = true;
	time = internal_timer;
}

void InGame_UI::ShowCheckpointSaved()
{
	checkpoint_saved_text->SetEnable(true);
	time_checkpoint = internal_timer;
}

void InGame_UI::StartLerpParticleUltibar(const float3& world_position)
{
	UI_Particles* particle = new UI_Particles();
	// not working very well but it's the best I accomplished
	//particle->origin_position = float3(ComponentCamera::WorldToScreenPoint(world_position).x/canvas->width, 
		//ComponentCamera::WorldToScreenPoint(world_position).y / canvas->height, 1);

	float random = Random::GetRandomIntBetweenTwo(1, 2);
	if (random == 1)
	{
		particle->origin_position = float3(-25.f, 43.f, -0.1f);
	}
	else
	{
		particle->origin_position = float3(25.f, 43.f, -0.1f);
	}

	particle->final_position = game_object->GetChild("InGame")->GetChild("Ulti_bar")->transform->GetLocalPosition();
	particle->particle = GameObject::Instantiate(ulti_particle, particle->origin_position, false, in_game);
	particle->time_passed = internal_timer;

	particles.push_back(particle);
}

void InGame_UI::ShowUltiFilter(bool show)
{
	ulti_active = show;
	changing_alpha_filter = true;
	time_ulti_filter = internal_timer;
}

void InGame_UI::LoadActiveRelics()
{
	std::vector <std::vector <ComponentImage*>>* vector_images;

	auto player = GameManager::instance->player_manager->players.begin();
	for (; player != GameManager::instance->player_manager->players.end(); ++player)
	{
		if ((*player)->controller_index == 1)
		{
			vector_images = &geralt_relics;
		}
		else
		{
			vector_images = &yennefer_relics;
		}

		auto effect = (*player)->effects.begin();
		for (; effect != (*player)->effects.end(); ++effect)
		{
			if (dynamic_cast<AttackEffect*>((*effect)))
			{
				const char* string_element = dynamic_cast<AttackEffect*>((*effect))->element.c_str();

				for (auto attack = (*player)->attacks->attacks.begin(); attack != (*player)->attacks->attacks.end(); ++attack)
				{
					if (strcmp(string_element, "Fire") == 0)
					{
						(*vector_images)[attack - (*player)->attacks->attacks.begin()][0]->current_color.a = 1;
					}
					else if (strcmp(string_element, "Poison") == 0)
					{
						(*vector_images)[attack - (*player)->attacks->attacks.begin()][1]->current_color.a = 1;
					}
					else if (strcmp(string_element, "Lightning") == 0)
					{
						(*vector_images)[attack - (*player)->attacks->attacks.begin()][2]->current_color.a = 1;
					}
					else if (strcmp(string_element, "Poison") == 0)
					{
						(*vector_images)[attack - (*player)->attacks->attacks.begin()][3]->current_color.a = 1;
					}
					else if (strcmp(string_element, "Earth") == 0)
					{
						(*vector_images)[attack - (*player)->attacks->attacks.begin()][4]->current_color.a = 1;
					}
				}
			}
			else
			{
				const char* string = dynamic_cast<DashEffect*>((*effect))->element.c_str();

				if (strcmp(string, "Fire") == 0)
				{
					(*vector_images)[4][0]->current_color.a = 1;
				}
				else if (strcmp(string, "Poison") == 0)
				{
					(*vector_images)[4][1]->current_color.a = 1;
				}
				else if (strcmp(string, "Lightning") == 0)
				{
					(*vector_images)[4][2]->current_color.a = 1;
				}
				else if (strcmp(string, "Poison") == 0)
				{
					(*vector_images)[4][3]->current_color.a = 1;
				}
				else if (strcmp(string, "Earth") == 0)
				{
					(*vector_images)[4][4]->current_color.a = 1;
				}
			}
		}
	}
}
