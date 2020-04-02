#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;

class ALIEN_ENGINE_API TriggerEnvironment : public Alien {
public:
	enum (Environment,
		QUIET,
		WIND,
		FOREST,
		BIRDS_WIND
		);
	TriggerEnvironment();
	virtual ~TriggerEnvironment();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);
	std::string GetNameByEnum(Environment mat);

public:
	Environment environment = Environment::QUIET;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	ComponentAudioEmitter* emitter = nullptr;
	GameObject* p1 = nullptr;
	GameObject* p2 = nullptr;
	int player_counter = 0;
	float timer = 0.f;
};
ALIEN_FACTORY TriggerEnvironment* CreateTriggerEnvironment() {
	TriggerEnvironment* alien = new TriggerEnvironment();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::Environment, alien->environment);
	return alien;
}
