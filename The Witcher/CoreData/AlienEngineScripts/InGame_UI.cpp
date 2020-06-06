#include "InGame_UI.h"
#include "PlayerController.h"
#include "UI_Char_Frame.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "UltiBar.h"
#include "Scores_Data.h"

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
}

void InGame_UI::Update()
{
	if (((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_START)) || (Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_START))||(Input::GetKeyDown(SDL_SCANCODE_ESCAPE)))&&!died)
	{
		PauseMenu(!Time::IsGamePaused());
	}

	if (checkpoint_saved_text->IsEnabled())
	{
		float t = (Time::GetGameTime() - time_checkpoint) / 0.5f;
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
				time_checkpoint = Time::GetGameTime() + 1.5f;
				break;
			}
			case CP_STATE::SHOW:
			{
				time_checkpoint = Time::GetGameTime();
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
		if (time + waiting < Time::GetGameTime())
		{
			if (!died_gone)
			{
				you_died->SetEnable(true);
				time = Time::GetGameTime();
				waiting = 2;
				died_gone = true;
			}
			else
			{
				if (strcmp(SceneManager::GetCurrentScene(), "Lvl_1_Tutorial") == 0)
				{
					SceneManager::LoadScene("Lvl_1_Tutorial");
				}
				else
				{
					Scores_Data::dead = true;
					Scores_Data::player1_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
					Scores_Data::player2_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;
					Scores_Data::player1_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->relics;
					Scores_Data::player2_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->relics;
					Scores_Data::last_scene = SceneManager::GetCurrentScene();
					SceneManager::LoadScene("NewWin_Menu");
				}
			}
		}
	}

	if (!particles.empty())
	{
		auto particle = particles.begin();
		for (; particle != particles.end(); ++particle)
		{
			float lerp = (Time::GetGameTime() - (*particle)->time_passed) / time_lerp_ult_part;
			float position_x = Maths::Lerp((*particle)->origin_position.x, (*particle)->final_position.x, lerp);
			float position_y = Maths::Lerp((*particle)->origin_position.y, (*particle)->final_position.y, lerp);

			(*particle)->particle->transform->SetLocalPosition(position_x, position_y, 1);

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
}

void InGame_UI::YouDied()
{
	died = true;
	time = Time::GetGameTime();
}

void InGame_UI::ShowCheckpointSaved()
{
	checkpoint_saved_text->SetEnable(true);
	time_checkpoint = Time::GetGameTime();
}

void InGame_UI::StartLerpParticleUltibar(const float3& world_position)
{
	UI_Particles* particle = new UI_Particles();
	// not working very well but it's the best I accomplished
	//particle->origin_position = float3(ComponentCamera::WorldToScreenPoint(world_position).x/canvas->width, 
		//ComponentCamera::WorldToScreenPoint(world_position).y / canvas->height, 1);

	particle->origin_position = float3(0, 43.f, 0);
	particle->final_position = game_object->GetChild("InGame")->GetChild("Ulti_bar")->transform->GetLocalPosition();
	particle->particle = GameObject::Instantiate(ulti_particle, particle->origin_position, false, in_game);
	particle->time_passed = Time::GetGameTime();

	particles.push_back(particle);
}

void InGame_UI::ShowUltiFilter(bool show)
{
	ulti_active = show;
	changing_alpha_filter = true;
	time_ulti_filter = Time::GetTimeSinceStart();
}
