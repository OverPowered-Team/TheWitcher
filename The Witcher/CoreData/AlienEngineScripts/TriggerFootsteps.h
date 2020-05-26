#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;

class ALIEN_ENGINE_API TriggerFootsteps : public Alien {
public:
	enum (GroundMaterial,
		MUD,
		SAND,
		STONE,
		VEGETATION,
		WOOD
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
	float4 terrain_particle_color; 
	float4 terrain_particle_color_final;
};
ALIEN_FACTORY TriggerFootsteps* CreateTriggerFootsteps() {
	TriggerFootsteps* alien = new TriggerFootsteps();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerFootsteps::GroundMaterial, alien->material_1);
	SHOW_TEXT("Player particles color associated with this terrain:");
	SHOW_TEXT("Initial Color");
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color.x, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color.y, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color.z, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color.w, 0.f, 1.f);
	SHOW_TEXT("Final Color");
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color_final.x, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color_final.y, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color_final.z, 0.f, 1.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->terrain_particle_color_final.w, 0.f, 1.f);

	return alien;
}
