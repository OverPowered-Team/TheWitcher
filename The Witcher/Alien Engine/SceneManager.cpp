#include "SceneManager.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ResourceScene.h"

int SceneManager::scenes_loaded = 0;

void SceneManager::LoadScene(const char* scene_name)
{
	App->objects->sceneNameToChange.assign(scene_name);
}

int SceneManager::ScenesLoaded()
{
	return scenes_loaded;
}

const char* SceneManager::GetCurrentScene()
{
	return App->objects->current_scene->GetName();
}