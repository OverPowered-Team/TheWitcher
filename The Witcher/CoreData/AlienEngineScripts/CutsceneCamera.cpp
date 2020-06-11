#include "CutsceneCamera.h"
#include "CutsceneShot.h"
#include "CameraMovement.h"
#include "CameraShake.h"
#include "PlayerController.h"

CutsceneCamera::CutsceneCamera() : Alien()
{
}

CutsceneCamera::~CutsceneCamera()
{
}

void CutsceneCamera::Start()
{
	camera = Camera::GetCurrentCamera()->game_object_attached;
	cam_movement = camera->GetComponent<CameraMovement>();
	cam_shaking = camera->GetComponent<CameraShake>();
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
	std::sort(shots.begin(), shots.end(), OrderCutscenes);
}

void CutsceneCamera::PrepareCutscene()
{
	if (shots_counter >= shots.size())
	{
		cam_movement->state = cam_movement->prev_state;
		cam_movement->is_cinematic = false;
		cam_movement->players[0]->GetComponent<PlayerController>()->input_blocked = false;
		cam_movement->players[1]->GetComponent<PlayerController>()->input_blocked = false;
	}
	else {
		state = CutsceneState::MOVING;//Yes I know, this is hardcoded but lets be honest, we will not do the 1 player mode
		cam_movement->players[0]->GetComponent<PlayerController>()->input_blocked = true;
		cam_movement->players[1]->GetComponent<PlayerController>()->input_blocked = true;
	}
}

void CutsceneCamera::ExecuteCutscene()
{
	switch (state)
	{
	case CutsceneState::IDLE:
	{
		if (Time::GetGameTime() - shots[shots_counter]->element.stay_timer >= shots[shots_counter]->element.stay_time)
		{
			shots_counter++;
			PrepareCutscene();
			LOG("COUNTING %i", shots_counter);
			return;
		}
		if (shots[shots_counter]->element.it_shake && Time::GetGameTime() - shots[shots_counter]->element.info_shake.shake_time >= shots[shots_counter]->element.info_shake.shake_time && !shots[shots_counter]->element.info_shake.has_shaked)
		{
			cam_shaking->Shake(shots[shots_counter]->element.info_shake.strength, shots[shots_counter]->element.info_shake.traumaDecay, shots[shots_counter]->element.info_shake.off_set, shots[shots_counter]->element.info_shake.maxyaw, shots[shots_counter]->element.info_shake.maxpitch, shots[shots_counter]->element.info_shake.maxroll);
			shots[shots_counter]->element.info_shake.has_shaked = true;
			LOG("IM SHAKING");
		}		
		break;
	}
	case CutsceneState::MOVING:
	{
		float min_dist = 0.1f;
		current_move_time += Time::GetDT();
		
		if ((shots[shots_counter]->transform->GetGlobalPosition() - Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition()).Length() < min_dist) {
			Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(shots[shots_counter]->transform->GetGlobalPosition());
			current_move_time = 0.f;
			shots[shots_counter]->element.stay_timer = Time::GetGameTime();
			if(shots[shots_counter]->element.it_shake)
				shots[shots_counter]->element.info_shake.shake_timer = Time::GetGameTime();
			state = CutsceneState::IDLE;
			LOG("IM VIBING");
		}
		else
		{ //Move postion
			if (shots[shots_counter]->element.first_frame)
			{
				if (!shots[shots_counter]->element.it_focus)
					shots[shots_counter]->element.first_frame = false;
				shots[shots_counter]->element.first_pos = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition();
				t_speed = Time::GetGameTime();
			}
			float3 current_pos = float3::Lerp(shots[shots_counter]->element.first_pos, shots[shots_counter]->transform->GetGlobalPosition(), (Time::GetGameTime() - t_speed) / shots[shots_counter]->element.transition_speed);
			Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(current_pos);
		}
		
		if (shots[shots_counter]->element.it_focus && shots[shots_counter]->element.g_o_focus) { //Move rotation
			if (shots[shots_counter]->element.first_frame)
			{
				shots[shots_counter]->element.first_frame = false;
				shots[shots_counter]->element.first_rot = camera->transform->GetGlobalRotation();
				float3 direction = (shots[shots_counter]->element.g_o_focus->transform->GetGlobalPosition() - shots[shots_counter]->transform->GetGlobalPosition()).Normalized();
				shots[shots_counter]->element.final_rot = Quat::LookAt(float3::unitZ(), direction, float3::unitY(), float3::unitY());
			}
			Quat current_rot = Quat::Slerp(shots[shots_counter]->element.first_rot, shots[shots_counter]->element.final_rot, (Time::GetGameTime() - t_speed)/ shots[shots_counter]->element.transition_speed);
			camera->transform->SetGlobalRotation(current_rot);
			if ((Time::GetGameTime() - t_speed) / 2.f >= 1.f) {
				float3 direction = (shots[shots_counter]->element.g_o_focus->transform->GetGlobalPosition() - camera->transform->GetGlobalPosition()).Normalized();
				camera->transform->SetGlobalRotation(shots[shots_counter]->element.final_rot);
			}
		}
		break;
	}
	}
}

bool CutsceneCamera::OrderCutscenes(CutsceneShot* i, CutsceneShot* j)
{
	return (i->element.id_shot < j->element.id_shot); 
}
