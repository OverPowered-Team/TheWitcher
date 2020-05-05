#include "InGame_UI.h"
#include "PlayerController.h"
#include "UI_Char_Frame.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "UltiBar.h"

InGame_UI::InGame_UI() : Alien()
{
}

InGame_UI::~InGame_UI()
{
}

void InGame_UI::Start()
{
	pause_menu->SetEnable(false);
	GameObject::FindWithName("Menu")->SetEnable(true);
	canvas = game_object->GetComponent<ComponentCanvas>();
	you_died = GameObject::FindWithName("YouDied");
	relics_panel = GameObject::FindWithName("Relics_Notification");
	relics_panel->SetEnable(false);
	you_died->SetEnable(false);
	in_game->SetEnable(true);
	ulti_bar = game_object->GetChild("InGame")->GetChild("Ulti_Bar")->GetComponent<UltiBar>();
	checkpoint_saved_text = in_game->GetChild("NewCheckpoint");
	checkpoint_saved_text->SetEnable(false);
}

void InGame_UI::Update()
{
	if (((Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_START)) || (Input::GetControllerButtonUp(2, Input::CONTROLLER_BUTTON_START))||(Input::GetKeyDown(SDL_SCANCODE_ESCAPE)))&&!died)
	{
		PauseMenu(!Time::IsGamePaused());
	}

	if (checkpoint_saved_text->IsEnabled())
	{
		if (time_checkpoint + 2.f <= Time::GetGameTime())
		{
			checkpoint_saved_text->SetEnable(false);
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
				SceneManager::LoadScene("NewWin_Menu");
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
				if ((*particle)->player != nullptr && (*particle)->type == UI_Particle_Type::KILL_COUNT)
				{
					if ((*particle)->player->player_data.total_kills >= 10)
					{
						(*particle)->player->HUD->GetComponent<UI_Char_Frame>()->kill_count_number->SetText(
							std::to_string((*particle)->player->player_data.total_kills).c_str());
					}
					else
					{
						std::string kills = "0" + std::to_string((*particle)->player->player_data.total_kills);
						(*particle)->player->HUD->GetComponent<UI_Char_Frame>()->kill_count_number->SetText(kills.c_str());
						(*particle)->player->HUD->GetComponent<UI_Char_Frame>()->kill_count_number->SetBackgroundColor(0.961f, 0.961f, 0.961f, 1);
						(*particle)->player->HUD->GetComponent<UI_Char_Frame>()->StartFadeKillCount();
					}
				}
				else if((*particle)->type == UI_Particle_Type::ULTI)
				{
					float new_value = (float)GameManager::instance->player_manager->collective_ultimate_charge / (float)GameManager::instance->player_manager->max_ultimate_charge;
					if (new_value != 1)
					{
						ulti_bar->UpdateBar(new_value);
					}
					else
					{
						ulti_bar->MaxBar();
					}
				}

				GameObject::Destroy((*particle)->particle);
				(*particle) = nullptr;
				particles.erase(particle);
				--particle;
			}
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

void InGame_UI::StartLerpParticle(const float3& world_position, UI_Particle_Type type, PlayerController* player)
{
	UI_Particles* particle = new UI_Particles();
	// not working very well but it's the best I accomplished
	//particle->origin_position = float3(ComponentCamera::WorldToScreenPoint(world_position).x/canvas->width, 
		//ComponentCamera::WorldToScreenPoint(world_position).y / canvas->height, 1);

	particle->origin_position = float3(0, 0, 0);
	particle->type = type;

	switch (type)
	{
	case UI_Particle_Type::ULTI:
	{
		particle->final_position = game_object->GetChild("InGame")->GetChild("Ulti_bar")->transform->GetLocalPosition();
		particle->particle = GameObject::Instantiate(ulti_particle, particle->origin_position, false, in_game);
		break;
	}
	case UI_Particle_Type::KILL_COUNT:
	{
		particle->final_position = player->HUD->GetChild("Killcount")->transform->GetGlobalPosition();
		particle->particle = GameObject::Instantiate(killcount_particle, particle->origin_position, false, in_game);
		player->HUD->GetComponent<UI_Char_Frame>()->kill_count->SetEnable(true);
		particle->player = player;
		break;
	}
	}
	
	particle->time_passed = Time::GetGameTime();

	particles.push_back(particle);
}
