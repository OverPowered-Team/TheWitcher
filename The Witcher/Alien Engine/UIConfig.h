#pragma once

class ComponentAudioEmitter;

class __declspec(dllexport) UIConfig {
public:
	static float volume;
	static bool fullscreen;

	static void AddVolume(ComponentAudioEmitter* emitter);
	static void SubtractVolume(ComponentAudioEmitter* emitter);
	static void FullScreen();
};