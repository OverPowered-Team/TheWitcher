#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API MusicController : public Alien {

public:

	MusicController();
	virtual ~MusicController();
	
	void Start();
	void Update();
	void CleanUp();
	void EnemyInSight(Enemy* en);
	void EnemyLostSight(Enemy* en);
public:
	ComponentAudioEmitter* emitter = nullptr;
	bool is_combat = false;
	bool has_changed = false;
	std::string last_music;
	//int enemies_in_sight = 0;
	std::list<Enemy*>enemies_in_sight;
};

ALIEN_FACTORY MusicController* CreateMusicController() {
	MusicController* alien = new MusicController();
	// To show in inspector here

	return alien;
} 
