#include "LoadingBackground.h"

LoadingBackground::LoadingBackground() : Alien()
{
}

LoadingBackground::~LoadingBackground()
{
}

void LoadingBackground::Start()
{
	switch (sceneToChange) {
	case SceneChanges::TutorialToLvl1: {
		SceneManager::LoadSceneAsync("Lvl_1");
		break; }
	case SceneChanges::BalanToLvl2: {
		SceneManager::LoadSceneAsync("Mahakam_1");
		break; }
	case SceneChanges::Lv2ToWagon: {
		SceneManager::LoadSceneAsync("Wagonnetes");
		break; }
	case SceneChanges::WagonToCiri: {
		SceneManager::LoadSceneAsync("boss_test");
		break; }
	case SceneChanges::ForceTutorial: {
		SceneManager::LoadSceneAsync("Lvl_1_Tutorial");
		break; }
	}

	anim = GameObject::FindWithName("Fire")->GetComponent<ComponentAnimatedImage>();
	text = GameObject::FindWithName("Text")->GetComponent<ComponentText>();
}

void LoadingBackground::Update()
{
	if (change_next) {
		SceneManager::ChangeToAsyncScene();
	}

	if (!lerping) {
		if (SceneManager::IsSceneAsyncReady()) {
			lerping = true;
			time = Time::GetGameTime();
		}
	}
	else {
		if (anim->current_color.a <= 0.0F) {
			anim->current_color.a = 0.0F;
			text->SetAlpha(0.0F);
			change_next = true;
			return;
		}
		float t = (Time::GetGameTime() - time) / 1.0F;
		float lerp = Maths::Lerp(1.0F, 0.0F, t);
		anim->current_color.a = lerp;
		text->SetAlpha(lerp);
	}
}