#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MusicController : public Alien {

public:

	MusicController();
	virtual ~MusicController();
	
	void Start();
	void Update();
	
public:
	ComponentAudioEmitter* emitter = nullptr;
	bool is_combat = false;
	bool has_changed = false;
	std::string last_music;
};

ALIEN_FACTORY MusicController* CreateMusicController() {
	MusicController* alien = new MusicController();
	// To show in inspector here

	return alien;
} 
