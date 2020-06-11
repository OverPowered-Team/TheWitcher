#include "TestingThread.h"

TestingThread::TestingThread() : Alien()
{
}

TestingThread::~TestingThread()
{
}

void TestingThread::Start()
{
	SceneManager::LoadSceneAsync("Lvl_1");
}

void TestingThread::Update()
{
	if (SceneManager::IsSceneAsyncReady()) {
		LOG("READYY");
	}
	if (Input::GetKeyDown(SDL_SCANCODE_G)) {
		SceneManager::ChangeToAsyncScene();
	}
}