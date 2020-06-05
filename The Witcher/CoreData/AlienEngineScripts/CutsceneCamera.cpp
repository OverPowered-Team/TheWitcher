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
	std::sort(shots.begin(), shots.end());
}

void CutsceneCamera::PrepareCutscene()
{
	if (shots_counter > shots.size())
	{
		cam_movement->state = cam_movement->prev_state;
		cam_movement->is_cinematic = false;
	}
	else
		shots[shots_counter]->element.play_timer = Time::GetGameTime();
}

void CutsceneCamera::ExecuteCutscene()
{
	if (Time::GetGameTime() - shots[shots_counter]->element.play_timer >= shots[shots_counter]->element.play_time)
	{
		shots_counter++;
		PrepareCutscene();
		return;
	}

	switch (state)
	{
	case CutsceneState::MOVING:
	{
		float min_dist = 0.1f;
		current_move_time += Time::GetDT();

		if ((shots[shots_counter]->transform->GetGlobalPosition() - Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition()).Length() < min_dist) {
			Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(shots[shots_counter]->transform->GetGlobalPosition());
			current_move_time = 0.f;
			state = CutsceneState::IDLE;
		}
		else
		{
			float time_percent = (current_move_time / shots[shots_counter]->element.transition_speed);
			Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition() + (shots[shots_counter]->transform->GetGlobalPosition() - Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition()) * (time_percent));
		}
		break;
	}
	case CutsceneState::SHAKING:
	{
		break;
	}
	case CutsceneState::LOOKING:
	{
		break;
	}
	}
}
