#pragma once

#include "Module.h"
#include "Globals.h"
#include "SceneManager.h"

class GameObject;
class ComponentImage;
class ComponentCanvas;

struct Fade
{
	Fade() {}

	// Fade Types
	FadeType fade_type = FadeType::NONE;
	FadeToBlackType ftb_type = FadeToBlackType::NONE;

	// Time values
	float fading_time = 0.0f;
	float time_start = 0.0f;

	// Global Items
	GameObject* root_object = nullptr;
	float3 fade_color = { 0,0,0 };

	union
	{
		struct
		{
			ComponentImage* fading_image = nullptr;
			float origin_value = 0.0f;
			float final_value = 1.0f;
		}linear_fade;

		struct
		{
			ComponentImage* image1 = nullptr;
			ComponentImage* image2 = nullptr;
		}curtain;
	};
};


class FadeToBlack : public Module
{
public:
	FadeToBlack(bool start_enabled = false);
	~FadeToBlack();

	update_status PreUpdate(float dt);

	void StartFade(float seconds, FadeType fade_type, FadeToBlackType FTB_Type, float3 fade_color = float3(0, 0, 0), const char* scene_name_to_change = nullptr);

	void Reset();

private:

	void CreateComponentImage(ComponentCanvas* canvas, ComponentImage** c_image, float3 color);

private:
	Fade* fade = nullptr;
	bool fading_from = false;
	const char* scene_name = nullptr;
};
