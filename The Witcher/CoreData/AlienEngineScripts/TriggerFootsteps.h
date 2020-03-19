#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;

class ALIEN_ENGINE_API TriggerFootsteps : public Alien {
public:
	enum (GroundMaterial,
		MUD,
		SAND,
		STONE
		);
	TriggerFootsteps();
	virtual ~TriggerFootsteps();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);
	std::string GetNameByEnum(GroundMaterial mat);

public:
	GroundMaterial material_1 = GroundMaterial::SAND;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	ComponentAudioEmitter* emitter = nullptr;
};
ALIEN_FACTORY TriggerFootsteps* CreateTriggerFootsteps() {
	TriggerFootsteps* alien = new TriggerFootsteps();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerFootsteps::GroundMaterial, alien->material_1);
	return alien;
}
