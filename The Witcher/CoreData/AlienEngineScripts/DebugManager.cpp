#include "DebugManager.h"
#include "PlayerController.h"

DebugManager::DebugManager() : Alien()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Start()
{
	geralt_controller = GameObject::FindWithName("Geralt")->GetComponent<PlayerController>();
	yennefer_controller = GameObject::FindWithName("Yennefer")->GetComponent<PlayerController>();
	main_camera = Camera::GetCurrentCamera();
}

void DebugManager::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_F3))
	{
		if (Input::GetKeyDown(SDL_SCANCODE_M))
		{
			SceneManager::LoadScene("Main_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_W))
		{
			SceneManager::LoadScene("NewWin_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_L))
		{
			SceneManager::LoadScene("Mahakam");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_D))
		{
			SceneManager::LoadScene("EndGame_Menu");
		}
		if (Input::GetKeyDown(SDL_SCANCODE_F))
		{
			if (Camera::GetCurrentCamera() == main_camera)
			{
				ComponentCamera* aux_cam = GameObject::FindWithName("Aux Camera")->GetComponent<ComponentCamera>();
				if (aux_cam)
					Camera::SetCurrentCamera(aux_cam);
			}
			else
				Camera::SetCurrentCamera(main_camera);

		}
		if (Input::GetKeyDown(SDL_SCANCODE_G))
		{
			geralt_controller->godmode = !geralt_controller->godmode;
			yennefer_controller->godmode = !yennefer_controller->godmode;
			GameObject::FindWithName("Godmode")->SetEnable(!GameObject::FindWithName("Godmode")->IsEnabled());
		}
	}

}