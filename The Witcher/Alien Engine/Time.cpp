#include "Time.h"
#include "Timer.h"
#include "Application.h"
#include "PanelConsole.h"
#include "ModuleObjects.h"
#include "PanelGame.h"
#include "ModuleUI.h"
#include "ResourceScene.h"
#include "Resource_.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "PanelScene.h"
#include "Event.h"

Time::GameState Time::state = Time::GameState::NONE;
float Time::time_since_start = 0.0F;
float Time::game_time = 0.0F;
float Time::delta_time = 0.0F;
float Time::engine_dt = 0.0F;
bool Time::is_paused = false;
float Time::scale_time = 1.0F;
Timer* Time::start_timer = new Timer();
Timer* Time::game_timer = new Timer();

#include "mmgr/mmgr.h"

void Time::Start()
{
	start_timer->Start();
}

void Time::Update()
{
	time_since_start = start_timer->ReadSec();

	if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
		game_time = game_timer->ReadSec(scale_time);
	}
}

void Time::Play()
{
	static std::vector<std::string> actual_scene_names;
	if (state == GameState::NONE) {
#ifndef GAME_VERSION
		if (!App->objects->current_scenes.empty()) {
			for each (ResourceScene * scene in App->objects->current_scenes) {
				actual_scene_names.push_back(scene->GetName());
			}
		}
		App->objects->SaveScene(nullptr, "Library/play_scene.alienScene");
		App->objects->ignore_cntrlZ = true;
		if (App->ui->panel_console->clear_on_play) {
			App->game_string_logs.clear();
			App->engine_string_logs.clear();
			App->all_engine_logs.clear();
			App->all_game_logs.clear();
		}
		ImGui::SetWindowFocus(App->ui->panel_game->GetPanelName().data());
		App->ui->panel_console->game_console = true;
#endif
		state = GameState::PLAY;
		App->CastEvent(EventType::ON_PLAY);
		game_time = 0.0F;
		game_timer->Start();
	}
	else if (state == GameState::PAUSE) {
		state = GameState::PLAY;
		game_timer->Resume();
	}
	else if (state == GameState::PLAY) {
		App->objects->CleanUpScriptsOnStop();
		is_paused = false;
		state = GameState::NONE;
		App->CastEvent(EventType::ON_STOP);
		game_time = 0.0F;
		App->objects->LoadScene("Library/play_scene.alienScene", false);
		App->objects->ignore_cntrlZ = false;
		remove("Library/play_scene.alienScene");
#ifndef GAME_VERSION
		App->objects->errors = false;
		if (!actual_scene_names.empty()) {
			App->objects->current_scenes.clear();
			for (auto item = actual_scene_names.begin(); item != actual_scene_names.end(); ++item) {
				ResourceScene* scene = App->resources->GetSceneByName((*item).data());
				if (scene != nullptr) {
					bool exists = false;
					for (auto item = App->objects->current_scenes.begin(); item != App->objects->current_scenes.end(); ++item) {
						if ((*item) == scene) {
							exists = true;
							break;
						}
					}
					if (!exists) {
						App->objects->current_scenes.push_back(scene);
					}
				}
			}
		}
		App->ui->panel_console->game_console = false;
		ImGui::SetWindowFocus(App->ui->panel_scene->GetPanelName().data());
		actual_scene_names.clear();
#endif
	}
}

void Time::Pause()
{
	if (state == GameState::PAUSE) {
		Time::Play();
	}
	else if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
		state = GameState::PAUSE;
		App->CastEvent(EventType::ON_PAUSE);
		game_timer->Pause();
	}
}

void Time::PlayOnce()
{
	if (state == GameState::PAUSE) {
		game_timer->Resume();
		state = GameState::PLAY_ONCE;
	}
	else if (state == GameState::PLAY) {
		state = GameState::PLAY_ONCE;
	}
}

void Time::CleanUp()
{
	if (start_timer != nullptr)
		delete start_timer;
	if (game_timer != nullptr)
		delete game_timer;
}

Time::GameState Time::GetGameState()
{
	return state;
}

void Time::SetScaleTime(const float& scale)
{
	scale_time = scale;
}

void Time::SetDT(const float& dt)
{
	delta_time = dt;
}

float Time::GetCurrentDT()
{
	if (state == GameState::NONE)
		return engine_dt;
	else
		return delta_time * scale_time; // GetDT()
}

float Time::GetDT()
{
	return delta_time * scale_time;
}

float Time::GetTimeSinceStart()
{
	return time_since_start;
}

float Time::GetGameTime()
{
	return game_time;
}

float Time::GetScaleTime()
{
	return scale_time;
}

void Time::SetPause(bool is_pause)
{
	is_paused = is_pause;
}

bool Time::IsGamePaused()
{
	return is_paused;
}

bool Time::IsPlaying()
{
	if (state == GameState::PLAY || state == GameState::PLAY_ONCE)
		return true;
	else
		return false;
}

bool Time::IsPaused()
{
	return state == GameState::PAUSE;
}

bool Time::IsInGameState()
{
	return state != GameState::NONE;
}

void Time::Stop()
{
	game_time = 0.0F;
	state = GameState::NONE;
}
