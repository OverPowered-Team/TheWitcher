#include "SceneManager.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ResourceScene.h"
#include "ModuleFadeToBlack.h"
#include "ModuleResources.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

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

static int loadSceneInBackground(void* ptr)
{
	SDL_GL_MakeCurrent(App->window->window, App->renderer3D->back_context);

	App->objects->LoadSceneAsync(App->objects->toLoad.data());

	SDL_GL_MakeCurrent(App->window->window, NULL);

	SDL_AtomicIncRef(&App->objects->dataIsReady);
	return 0;
}


void SceneManager::LoadSceneAsync(const char* scene_name)
{
	App->objects->toLoad = scene_name;
	SDL_CreateThread(loadSceneInBackground, "LoadSceneAsync", (void*)NULL);
}

bool SceneManager::IsSceneAsyncReady()
{
	return SDL_AtomicGet(&App->objects->dataIsReady) == 1;
}

void SceneManager::ChangeToAsyncScene()
{
	if (SDL_AtomicGet(&App->objects->dataIsReady) == 1) {
		App->objects->changeToAsync = true;
		SDL_AtomicSet(&App->objects->dataIsReady, 0);
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
