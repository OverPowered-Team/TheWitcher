#include "ModuleFadeToBlack.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "AnimTween.h"
#include "Time.h"

FadeToBlack::FadeToBlack(bool start_enabled): Module(start_enabled)
{
	name.assign("FadeToBlack");
}

FadeToBlack::~FadeToBlack()
{
}

update_status FadeToBlack::PreUpdate(float dt)
{
	if (fade != nullptr)
	{
		float increment = (Time::GetTimeSinceStart() - fade->time_start) / fade->fading_time;
		
		switch (fade->ftb_type)
		{
		case FadeToBlackType::FADE:
		{
			fade->linear_fade.fading_image->SetBackgroundColor(fade->fade_color.x, fade->fade_color.y, fade->fade_color.z, 
				math::Lerp(fade->linear_fade.origin_value, fade->linear_fade.final_value, increment));
			break;
		}
		case FadeToBlackType::HORIZONTAL_CURTAIN:
		{
			if (fade->fade_type == FadeType::FADE_FROM)
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(-40, 0, 0), float3(-120, 0, 0), increment));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(40, 0, 0), float3(120, 0, 0), increment));
			}
			else
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(-120, 0, 0), float3(-38, 0, 0), increment));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(120, 0, 0), float3(38, 0, 0), increment));
			}
			break;
		}
		case FadeToBlackType::VERTICAL_CURTAIN:
		{
			if (fade->fade_type == FadeType::FADE_FROM)
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(0, -20, 0), float3(0, -67.5, 0), increment));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(0, 20, 0), float3(0, 67.5, 0), increment));
			}
			else
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(0, -67.5, 0), float3(0, -22.5, 0), increment));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(math::Lerp(float3(0, 67.5, 0), float3(0, 22.5, 0), increment));
			}
			break;
		}
		}

		if (increment > 1)
		{
			// Tween completed
			Reset();
		}
	}
	return UPDATE_CONTINUE;
}

void FadeToBlack::StartFade(float seconds, FadeType fade_type, FadeToBlackType FTB_Type, float3 fade_color, const char* scene_name_to_change)
{
	if (FTB_Type != FadeToBlackType::NONE && ((fade == nullptr) || fading_from))
	{
		if (fade == nullptr)
		{
			fade = new Fade();
		}
		fade->fade_type = fade_type;
		fade->ftb_type = FTB_Type;
		fade->fade_color = fade_color;
		scene_name = scene_name_to_change;

		if ((scene_name_to_change)||fade->fade_type==FadeType::COMPLETE_FADE)
		{
			fade->fading_time = seconds * 0.5f;
		}
		else
		{
			fade->fading_time = seconds;
		}

		// Fade Type
		switch (fade_type)
		{
		case FadeType::FADE_TO:
		{
			fade->linear_fade.origin_value = 0;
			fade->linear_fade.final_value = 1.0f;
			break;
		}
		case FadeType::FADE_FROM:
		{
			fade->linear_fade.origin_value = 1.0f;
			fade->linear_fade.final_value = 0;
			fading_from = false;
			break;
		}
		case FadeType::COMPLETE_FADE:
		{
			fade->linear_fade.origin_value = 0;
			fade->linear_fade.final_value = 1.0f;
			fading_from = true;
			break;
		}
		}

		// Create Canvas
		fade->root_object = App->objects->CreateEmptyGameObject(nullptr, false);
		fade->root_object->SetName("FadeToBlack");
		App->objects->MoveObjectUp(fade->root_object, true);
		ComponentCanvas* canvas = new ComponentCanvas(fade->root_object);
		fade->root_object->AddComponent(canvas);

		// Create Images
		switch (FTB_Type)
		{
		case FadeToBlackType::FADE:
		{
			CreateComponentImage(canvas, &fade->linear_fade.fading_image, float3(fade_color.x, fade_color.y, fade_color.z));
			break;
		}
		case FadeToBlackType::DIAGONAL_1:
		{
			CreateComponentImage(canvas, &fade->curtain.image1, float3(fade_color.x, fade_color.y, fade_color.z));
			CreateComponentImage(canvas, &fade->curtain.image2, float3(fade_color.x, fade_color.y, fade_color.z));
			break;
		}
		case FadeToBlackType::DIAGONAL_2:
		{
			CreateComponentImage(canvas, &fade->curtain.image1, float3(fade_color.x, fade_color.y, fade_color.z));
			CreateComponentImage(canvas, &fade->curtain.image2, float3(fade_color.x, fade_color.y, fade_color.z));
			break;
		}
		case FadeToBlackType::HORIZONTAL_CURTAIN:
		{
			CreateComponentImage(canvas, &fade->curtain.image1, float3(fade_color.x, fade_color.y, fade_color.z));
			CreateComponentImage(canvas, &fade->curtain.image2, float3(fade_color.x, fade_color.y, fade_color.z));
			if (fade->fade_type == FadeType::FADE_FROM)
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(float3(-38, 0, 0));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(float3(38, 0, 0));
			}
			else
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(float3(-125, 0, 0));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(float3(125, 0, 0));
			}
			break;
		}
		case FadeToBlackType::VERTICAL_CURTAIN:
		{
			CreateComponentImage(canvas, &fade->curtain.image1, float3(fade_color.x, fade_color.y, fade_color.z));
			CreateComponentImage(canvas, &fade->curtain.image2, float3(fade_color.x, fade_color.y, fade_color.z));
			if (fade->fade_type == FadeType::FADE_FROM)
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(float3(0, -22.5, 0));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(float3(0, 22.5, 0));
			}
			else
			{
				fade->curtain.image1->game_object_attached->transform->SetLocalPosition(float3(0, -67.5, 0));
				fade->curtain.image2->game_object_attached->transform->SetLocalPosition(float3(0, 67.5, 0));
			}
			break;
		}
		}

		// Enable Module to start PreUpdate
		if (!IsEnabled())
		{
			SetEnable(true);
		}
		fade->time_start = Time::GetTimeSinceStart();
	}
}

void FadeToBlack::Reset()
{
	GameObject::DestroyInstantly(fade->root_object);

	if (scene_name)
	{
		App->objects->LoadScene(scene_name);
		fading_from = true;
	}

	if (fading_from)
	{
		StartFade(fade->fading_time, FadeType::FADE_FROM, fade->ftb_type, fade->fade_color);
	}
	else
	{
		delete fade;
		fade = nullptr;
		SetEnable(false);
	}
}

void FadeToBlack::CreateComponentImage(ComponentCanvas* canvas, ComponentImage** c_image, float3 color)
{
	// Create Image and Set Component
	GameObject* image = new GameObject(fade->root_object);
	image->SetName("Fade");
	*c_image = new ComponentImage(image);
	image->AddComponent(*c_image);
	(*c_image)->SetCanvas(canvas);
	switch (fade->ftb_type)
	{
	case FadeToBlackType::FADE:
	{
		(*c_image)->SetBackgroundColor(color.x, color.y, color.z, fade->linear_fade.origin_value);
		image->transform->SetLocalScale(float3(80, 45, 1));
		break;
	}
	case FadeToBlackType::HORIZONTAL_CURTAIN:
	{
		(*c_image)->SetBackgroundColor(color.x, color.y, color.z, 1);
		image->transform->SetLocalScale(float3(45, 45, 1));
		break;
	}
	case FadeToBlackType::VERTICAL_CURTAIN:
	{
		(*c_image)->SetBackgroundColor(color.x, color.y, color.z, 1);
		image->transform->SetLocalScale(float3(80, 25, 1));
		break;
	}
	}
	
}
