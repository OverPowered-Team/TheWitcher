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
	emitter->SetState("Interactive_Music_Lvl1", "Quiet");
	last_music = "Quiet";
}

void MusicController::Update()
{
	if (is_combat && has_changed && enemies_in_sight.size() == 1)
	{
		emitter->SetState("Interactive_Music_Lvl1", "Combat");
		has_changed = !has_changed;
	}
	else if(!is_combat && has_changed && enemies_in_sight.size() <= 0)
	{
		emitter->SetState("Interactive_Music_Lvl1", last_music.c_str());
		has_changed = !has_changed;
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
