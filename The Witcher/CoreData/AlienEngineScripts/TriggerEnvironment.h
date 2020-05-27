#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;

class ALIEN_ENGINE_API TriggerEnvironment : public Alien {
public:
	enum(EnvironmentZone,
		QUIET,
		FORTRESS,
		RAVINE,
		MIDDLE_AREA,
		GHOUL_NESTS,
		DROWNERS,
		SLOPE_TO_CAVE,
		CAVE,
		PRE_BOSS,
		AFTER_BOSS
		);
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
		TREES_SQUEAKING,
		DRIPPING,
		ROCKS_FALLING,
		MINECART
		);
	struct MovingSpatial {
		GameObject* spatial_place = nullptr;
		GameObject* spatial_end = nullptr;
		float3 begin_pos;
		float3 direction_vec;
		ComponentAudioEmitter* el_emitter = nullptr;
		bool move = false;
		float curr_move_time = 0.f;
		float move_time = 1.f;
	};
	struct EnvironmentElement {
		AudioEffects type = AudioEffects::QUIET;
		bool spatial = false;
		bool spatial_movement = false;
		bool random = false;
		bool instant = false;
		bool can_play = false;
		float min_time_between_plays = 1.f;
		float max_time_between_plays = 1.f;
		float timer_play = 0.f;
		float timer_play_instant = 0.f;
		float time_to_play = 0.f;
		float time_to_play_instant = 0.f;
		MovingSpatial spatial_place;
		std::string event_name;
	};
	

	TriggerEnvironment();
	virtual ~TriggerEnvironment();
	void Start();
	void Update();
	void CleanUp();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);
	void OnDrawGizmosSelected()override;
	std::string GetAudioElementByEnum(AudioEffects eff);
	std::string GetZoneByEnum(EnvironmentZone zone);
	void PrepareEnvironmentElements();
	void PlayEnvironment();
	void PlayInstant();
	void MoveSpatial();
public:
	EnvironmentElement audio1;
	EnvironmentElement audio2;
	EnvironmentElement audio3;
	EnvironmentElement audio4;
	EnvironmentElement audio5;
	EnvironmentZone zone = EnvironmentZone::QUIET;
	ComponentAudioEmitter* emitter = nullptr;
	GameObject* p1 = nullptr;
	GameObject* p2 = nullptr;
	std::vector<EnvironmentElement> env_elements;
	int player_counter = 0;
};
ALIEN_FACTORY TriggerEnvironment* CreateTriggerEnvironment() {
	TriggerEnvironment* alien = new TriggerEnvironment();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::EnvironmentZone, alien->zone);
	SHOW_TEXT("Choose an audio event to reproduce in your environment");
	SHOW_TEXT("Only choose spatial if you know your sound IS in fact spatial");
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio1");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio1.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.spatial);
	SHOW_TEXT("SpatialMovement only if your spatial audio is selected and it will move");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.spatial_movement);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio1.spatial_place.move_time);
	SHOW_TEXT("If you choose instant can't be random!");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.instant);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio1.time_to_play_instant);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio1.random);
	SHOW_TOOLTIP("If it is NOT random we assume that sound will play continuously");
	SHOW_TEXT("If is Random, maximum and minimum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio1.min_time_between_plays);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio1.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio2");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio2.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.spatial);
	SHOW_TEXT("SpatialMovement only if your spatial audio is selected and it will move");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.spatial_movement);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio2.spatial_place.move_time);
	SHOW_TEXT("If you choose instant can't be random!");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.instant);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio2.time_to_play_instant);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio2.random);
	SHOW_TEXT("If is Random, maximum and minimum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio2.min_time_between_plays);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio2.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio3");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio3.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.spatial);
	SHOW_TEXT("SpatialMovement only if your spatial audio is selected and it will move");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.spatial_movement);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio3.spatial_place.move_time);
	SHOW_TEXT("If you choose instant can't be random!");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.instant);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio3.time_to_play_instant);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio3.random);
	SHOW_TEXT("If is Random, maximum and minimum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio3.min_time_between_plays);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio3.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio4");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio4.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.spatial);
	SHOW_TEXT("SpatialMovement only if your spatial audio is selected and it will move");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.spatial_movement);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio4.spatial_place.move_time);
	SHOW_TEXT("If you choose instant can't be random!");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.instant);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio4.time_to_play_instant);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio4.random);
	SHOW_TEXT("If is Random, maximum and minimum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio4.min_time_between_plays);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio4.max_time_between_plays);
	SHOW_SEPARATOR();
	SHOW_TEXT("Audio5");
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerEnvironment::AudioEffects, alien->audio5.type);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.spatial);
	SHOW_TEXT("SpatialMovement only if your spatial audio is selected and it will move");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.spatial_movement);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio5.spatial_place.move_time);
	SHOW_TEXT("If you choose instant can't be random!");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.instant);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio5.time_to_play_instant);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->audio5.random);
	SHOW_TEXT("If is Random, maximum and minimum time between plays");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio5.min_time_between_plays);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->audio5.max_time_between_plays);

	return alien;
}
