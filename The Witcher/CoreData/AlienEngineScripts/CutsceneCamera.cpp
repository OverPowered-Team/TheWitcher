#include "CutsceneCamera.h"
#include "CutsceneShot.h"
#include "CameraMovement.h"

CutsceneCamera::CutsceneCamera() : Alien()
{
}

CutsceneCamera::~CutsceneCamera()
{
}

void CutsceneCamera::Start()
{
	cam_movement = Camera::GetCurrentCamera()->game_object_attached->GetComponent<CameraMovement>();
	BuildCutscene();
}

void CutsceneCamera::Update()
{
}

void CutsceneCamera::CleanUp()
{
	shots.clear();
}

void CutsceneCamera::BuildCutscene()
{
	vector<GameObject*> childs = this->game_object->GetChildren();
	for (auto iter = childs.begin(); iter != childs.end(); ++iter)
	{
		CutsceneShot* shot = (*iter)->GetComponent<CutsceneShot>();
		if (shot)
			shots.push_back(shot);
	}

	//Order shots based in ID
	//for (int i = 0; i < shots.size(); ++i)
	//{
		std::sort(shots.begin(), shots.end());
	//}
		for (int i = 0; i < shots.size(); ++i)
		{
			LOG("%i : %i", i,shots[i]->element.id_shot);
		}
}

void CutsceneCamera::PrepareCutscene()
{

}

void CutsceneCamera::ExecuteCutscene()
{
	for (auto iter = shots.begin(); iter != shots.end(); ++iter) 
	{

	}
}
