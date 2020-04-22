#pragma once

enum class FadeType;
enum class FadeToBlackType;

#include "MathGeoLib/include/Math/float3.h"

class __declspec(dllexport) SceneManager {
public:
	static void LoadScene(const char* scene_name, FadeToBlackType ftb_type = (FadeToBlackType)-1, float fade_seconds = 1.0f, float3 color = { 0.0f,0.0f,0.0f });
	static int ScenesLoaded();

	// This function does not change the scene, only does a fade. Use LoadScene to change the scene with a Fade
	static void StartFade(float seconds, FadeType fade_type, FadeToBlackType ftb_type, float3 color);
private:

	static int scenes_loaded;
};
