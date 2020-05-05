#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;

class ALIEN_ENGINE_API TriggerEnvironment : public Alien {
public:
	enum (AudioEffects,
		QUIET,
		WIND,
		FOREST,
		BIRDS,
		CROWS,
		SQUIRRELS,
		RIVER,
		GLOOMY,
		INSECTS,
		TREE_FALL,
		WOLF_HOWLING,
		BATS,
		CAVE,
		TREES_SQUEAKING
		);
	struct EnvironmentElement {
		AudioEffects type = AudioEffects::QUIET;
		bool spatial = false;
		bool random = false;
		float max_time_between_plays = 1.f;
		GameObject* spatial_place = nullptr;
	};
	

	TriggerEnvironment();
	virtual ~TriggerEnvironment();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);
	std::string GetNameByEnum(AudioEffects eff);

public:
	EnvironmentElement audio1;
	EnvironmentElement audio2;
	EnvironmentElement audio3;
	EnvironmentElement audio4;
	EnvironmentElement audio5;

	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	ComponentAudioEmitter* emitter = nullptr;
	GameObject* p1 = nullptr;
	GameObject* p2 = nullptr;
	int player_counter = 0;
};
ALIEN_FACTORY TriggerEnvironment* CreateTriggerEnvironment() {
	TriggerEnvironment* alien = new TriggerEnvironment();
	// To show in inspector here
	SHOW_TEXT("Choose an audio event to reproduce in your environment");
	SHOW_TEXT("Only choose spatial if you know your sound IS in fact spatial");
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio1");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio1.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.spatial);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.random);
	SHOW_TEXT("If is Spatial");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->audio1.spatial_place);
	SHOW_TEXT("If is Random, maximum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio1.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio2");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio2.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.spatial);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.random);
	SHOW_TEXT("If is Spatial");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->audio2.spatial_place);
	SHOW_TEXT("If is Random, maximum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio2.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio3");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio3.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.spatial);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.random);
	SHOW_TEXT("If is Spatial");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->audio3.spatial_place);
	SHOW_TEXT("If is Random, maximum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio3.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio4");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio4.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.spatial);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.random);
	SHOW_TEXT("If is Spatial");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->audio4.spatial_place);
	SHOW_TEXT("If is Random, maximum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio4.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio5");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio5.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.spatial);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.random);
	SHOW_TEXT("If is Spatial");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->audio5.spatial_place);
	SHOW_TEXT("If is Random, maximum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio5.max_time_between_plays);
	return alien;
}
