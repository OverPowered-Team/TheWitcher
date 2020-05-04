#include "MusicController.h"
#include "Enemy.h"

MusicController::MusicController() : Alien()
{
}

MusicController::~MusicController()
{
}

void MusicController::Start()
{
	emitter = this->GetComponent<ComponentAudioEmitter>();
	emitter->SetState("Interactive_Music_Lvl1", "Combat");
	last_music = "Quiet";
	t1 = Time::GetGameTime();
}

void MusicController::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_U))
		emitter->SetState("Interactive_Music_Lvl1", "Mountain");

	if (is_combat)
	{
		if (has_changed && enemies_in_sight.size() == 1) {
			emitter->SetState("Interactive_Music_Lvl1", "Combat");
			has_changed = !has_changed;
		}
		DecreaseMusicVolume();
	}
	else if(!is_combat && has_changed && enemies_in_sight.size() <= 0)
	{
		emitter->SetState("Interactive_Music_Lvl1", last_music.c_str());
		has_changed = !has_changed;
		already_minium = false;
	}
	
}

void MusicController::CleanUp()
{
	enemies_in_sight.clear();
}

void MusicController::EnemyInSight(Enemy* en)
{
	is_combat = true;
	has_changed = true;
	if (enemies_in_sight.size() == 0) {
		enemies_in_sight.push_back(en);
		return;
	}
	else {
		for (std::list<Enemy*>::iterator it = enemies_in_sight.begin(); it != enemies_in_sight.end(); ++it)
		{
			if ((*it)->game_object != en->game_object) {
				enemies_in_sight.push_back(en);
				break;
			}
		}
	}
}

void MusicController::EnemyLostSight(Enemy* en)
{
	is_combat = false;
	has_changed = true;
	enemies_in_sight.remove(en);
}

void MusicController::DecreaseMusicVolume()
{
//AQUI LLAMAR VALOR RTPC
	if (Time::GetGameTime() - t1 >= 1.f && !already_minium) {
		dist += 0.1;
		emitter->SetRTPCValue("CombatDistance", dist);
		if (dist >= 100) {
			dist = 0.f;
			t1 = Time::GetGameTime();
			LOG("Minimum combat volume");
			already_minium = true;
			return;
		}
			
	}
}
