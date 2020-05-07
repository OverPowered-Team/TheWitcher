#include "MusicController.h"
#include "Enemy.h"

MusicController::MusicController() : Alien()
{
}

MusicController::~MusicController()
{
}

void MusicController::Awake()
{
	emitter = this->GetComponent<ComponentAudioEmitter>(); //Its ugly I know
	emitter->StartSound("Play_Bad_News_Ahead");
	emitter->StartSound("Play_Combat_Music");
	emitter->StartSound("Play_KaerMorhen");
	emitter->StartSound("Play_Ladies_of_the_Woods");
	emitter->StartSound("Play_No_Surrender");
	emitter->StartSound("Play_Random_Level1_Music");
	emitter->StartSound("Play_Trial_Of_The_Grasses");
	emitter->SetState("Interactive_Music_Lvl1", "Quiet");
	last_music = "Quiet";
}

void MusicController::Start()
{

	t1 = Time::GetGameTime();
}

void MusicController::Update()
{
	if (is_combat) // This should be done better with a switch about enemy types
	{
		if (has_changed && enemies_in_sight.size() == 1) {
			if ((*enemies_in_sight.begin())->game_object->GetComponent<Enemy>()->type == EnemyType::LESHEN)
			{
				emitter->SetState("Interactive_Music_Lvl1", "Boss");
			}
			else
				emitter->SetState("Interactive_Music_Lvl1", "Combat");

			has_changed = !has_changed;
		}
		DecreaseMusicVolume();
	}
	else if (!is_combat && has_changed && enemies_in_sight.size() <= 0)
	{
		emitter->SetState("Interactive_Music_Lvl1", last_music.c_str()); //CASE OF AFTER BOSS KILLING
		has_changed = !has_changed;
		already_minium = false;
		to_normal_rtpc = true;
		t2 = Time::GetGameTime();
	}
	if (to_normal_rtpc)
	{
		if (Time::GetGameTime() - t2 >= 1.5f) {
			dist = 0.f;
			emitter->SetRTPCValue("CombatDistance", dist);
			to_normal_rtpc = false;
		}
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
	if (Time::GetGameTime() - t1 >= 1.f && !already_minium) {
		dist += 0.1;
		emitter->SetRTPCValue("CombatDistance", dist);
		if (dist >= 100) {
			t1 = Time::GetGameTime();
			LOG("Minimum combat volume");
			already_minium = true;
		}

	}
}
