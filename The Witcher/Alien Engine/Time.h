#ifndef __TIME_H__
#define __TIME_H__

#include <string>

class Timer;

class __declspec(dllexport) Time {
public:

	enum class GameState {
		NONE,
		PLAY,
		PAUSE,
		PLAY_ONCE
	};

private:
	friend class Application;
	friend class ModuleUI;
	friend class ReturnZ;
	friend class ComponentCollider;
	friend class ComponentCharacterController;
	friend class ModuleObjects;
	friend class PanelScene;
	friend class PanelInspector;
	friend class ResourcePrefab;
	friend class ResourceAnimatorController;
	friend class ComponentAnimator;
	friend class PanelAnimTimeline;
	friend class ComponentAudioEmitter;
	friend class ModulePhysics;
	friend class ModuleAudio;
	friend class ComponentParticleSystem;
	friend class ComponentUI;
	friend class ComponentCheckbox;
	friend class ComponentSlider;
	friend class ComponentBar;
	friend class ComponentCanvas;

	static void Start();
	static void Update();

	static void Play();
	static void Pause();
	static void PlayOnce();
	static bool IsPlaying();
	static bool IsPaused();
	static bool IsInGameState();
	static void Stop();

	static void CleanUp();

	static void SetDT(const float& dt);
	static float GetCurrentDT();

private:

	static float delta_time;
	static float engine_dt;
	static Timer* game_timer;
	static Timer* start_timer;

	static GameState state;

	static float time_since_start;
	static float game_time;
	static float scale_time;


public:

	static GameState GetGameState();
	static void SetScaleTime(const float& scale);
	static float GetDT();
	static float GetTimeSinceStart();
	static float GetGameTime();
	static float GetScaleTime();
};

#endif
