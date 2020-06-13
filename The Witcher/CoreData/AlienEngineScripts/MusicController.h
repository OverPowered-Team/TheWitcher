#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;

class ALIEN_ENGINE_API MusicController : public Alien {

public:

	MusicController();
	virtual ~MusicController();
	void Awake();
	void Start();
	void Update();
	void CleanUp();
	void EnemyInSight(Enemy* en);
	void EnemyLostSight(Enemy* en);
	void DecreaseMusicVolume();
public:
	ComponentAudioEmitter* emitter = nullptr;
	bool is_combat = false;
	bool has_changed = false;
	bool is_lvl1 = true; // hacks
	std::string last_music;
	std::list<Enemy*>enemies_in_sight;
	float minimum_volume = 0.f;
	float time_start_decrease = 1.f;
private:
	float dist = 0.f;
	float t1 = 0.f;
	bool already_minium = false;
	bool to_normal_rtpc = false;
	float t2 = 0.f;
};

ALIEN_FACTORY MusicController* CreateMusicController() {
	MusicController* alien = new MusicController();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->minimum_volume);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->time_start_decrease);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->is_lvl1);
	return alien;
}
