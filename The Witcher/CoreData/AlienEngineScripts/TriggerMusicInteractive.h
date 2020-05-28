#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;
class MusicController;

class ALIEN_ENGINE_API TriggerMusicInteractive : public Alien {
public:
	enum (Music,
		QUIET,
		COMBAT,
		AFTER_BOSS,
		BOSS,
		CAVE,
		DROWNERS,
		FORTRESS,
		GHOUL_NESTS,
		MIDDLE_AREA,
		PRE_BOSS,
		RAVINE,
		SLOPE_TO_CAVE,
		MINE
		);
	TriggerMusicInteractive();
	virtual ~TriggerMusicInteractive();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	std::string GetNameByEnum(Music mat);

public:
	Music interactive = Music::QUIET;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	ComponentAudioEmitter* emitter = nullptr;
	MusicController* m_controller = nullptr;
	GameObject* p1 = nullptr;
	GameObject* p2 = nullptr;
	int player_counter = 0;
	bool first_time = true;
};
ALIEN_FACTORY TriggerMusicInteractive* CreateTriggerMusicInteractive() {
	TriggerMusicInteractive* alien = new TriggerMusicInteractive();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerMusicInteractive::Music, alien->interactive);
	return alien;
}
