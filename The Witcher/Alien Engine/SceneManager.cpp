#include "SceneManager.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ResourceScene.h"
#include "ModuleFadeToBlack.h"
#include "ModuleResources.h"

int SceneManager::scenes_loaded = 0;

void SceneManager::LoadScene(const char* scene_name, FadeToBlackType ftb_type, float fade_seconds, float3 color)
{
	if (ftb_type == FadeToBlackType::NONE)
	{
		App->objects->sceneNameToChange.assign(scene_name);
	}
	else
	{
		App->fade_to_black->StartFade(fade_seconds, FadeType::FADE_TO, ftb_type, color,scene_name);

	}
}


int SceneManager::ScenesLoaded()
{
	return scenes_loaded;
}

const char* SceneManager::GetCurrentScene()
{
	return App->resources->GetResourceWithID(App->objects->scene_active)->GetName();
}

void SceneManager::StartFade(float seconds, FadeType fade_type, FadeToBlackType ftb_type, float3 color)
{
	App->fade_to_black->StartFade(seconds, fade_type, ftb_type, color);
}
