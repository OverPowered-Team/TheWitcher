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

	for (auto iter = shots.begin(); iter != shots.end(); ++iter)
	{
		if ((*iter)->element.it_curve)
			BuildCurve((*iter));
	}

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
		if (!Time::IsGamePaused())
		{
			if (Time::GetGameTime() - shots[shots_counter]->element.stay_timer >= shots[shots_counter]->element.stay_time)
			{
				shots_counter++;
				PrepareCutscene();
				return;
			}
			if (shots[shots_counter]->element.it_shake && Time::GetGameTime() - shots[shots_counter]->element.info_shake.shake_timer >= shots[shots_counter]->element.info_shake.shake_time && !shots[shots_counter]->element.info_shake.has_shaked)
			{
				cam_shaking->Shake(shots[shots_counter]->element.info_shake.strength, shots[shots_counter]->element.info_shake.traumaDecay, shots[shots_counter]->element.info_shake.off_set, shots[shots_counter]->element.info_shake.maxyaw, shots[shots_counter]->element.info_shake.maxpitch, shots[shots_counter]->element.info_shake.maxroll);
				shots[shots_counter]->element.info_shake.has_shaked = true;
			}
		}
		break;
	}
	case CutsceneState::MOVING:
	{
		if(!Time::IsGamePaused())
		{
			if (shots[shots_counter]->element.it_curve)
			{
				ExecuteCurve();
			}
			else {
				if (shots[shots_counter]->element.first_frame)
				{
					if (!shots[shots_counter]->element.it_focus)
						shots[shots_counter]->element.first_frame = false;
					shots[shots_counter]->element.first_pos = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition();
					t_speed = Time::GetGameTime();
				}
				float3 current_pos = float3::Lerp(shots[shots_counter]->element.first_pos, shots[shots_counter]->transform->GetGlobalPosition(), (Time::GetGameTime() - t_speed) / shots[shots_counter]->element.transition_speed);
				Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(current_pos);

				if (shots[shots_counter]->element.it_focus && shots[shots_counter]->element.g_o_focus) { //Move rotation
					if (shots[shots_counter]->element.first_frame)
					{
						shots[shots_counter]->element.first_frame = false;
						shots[shots_counter]->element.first_rot = camera->transform->GetGlobalRotation();
						float3 direction = (shots[shots_counter]->element.g_o_focus->transform->GetGlobalPosition() - shots[shots_counter]->transform->GetGlobalPosition()).Normalized();
						shots[shots_counter]->element.final_rot = Quat::LookAt(float3::unitZ(), direction, float3::unitY(), float3::unitY());
					}
					Quat current_rot = Quat::Slerp(shots[shots_counter]->element.first_rot, shots[shots_counter]->element.final_rot, (Time::GetGameTime() - t_speed) / shots[shots_counter]->element.transition_speed);
					camera->transform->SetGlobalRotation(current_rot);
				}
				if ((Time::GetGameTime() - t_speed) / shots[shots_counter]->element.transition_speed >= 1.f) {
					Camera::GetCurrentCamera()->game_object_attached->transform->SetGlobalPosition(shots[shots_counter]->transform->GetGlobalPosition());
					if (shots[shots_counter]->element.it_focus && shots[shots_counter]->element.g_o_focus)
						camera->transform->SetGlobalRotation(shots[shots_counter]->element.final_rot);
					shots[shots_counter]->element.stay_timer = Time::GetGameTime();
					if (shots[shots_counter]->element.it_shake)
						shots[shots_counter]->element.info_shake.shake_timer = Time::GetGameTime();
					state = CutsceneState::IDLE;
					break;
				}
			}
		}	
		break;
	}
	}
}

void CutsceneCamera::ExecuteCurve()
{
	if (!shots[shots_counter]->element.curve_info.ended_intro && shots[shots_counter]->element.curve_info.curve && shots[shots_counter]->element.curve_info.cam)
	{
		if (shots[shots_counter]->element.first_frame && shots[shots_counter]->element.curve_info.cam)
		{
				shots[shots_counter]->element.curve_info.start_curve = shots[shots_counter]->element.curve_info.curve->curve.ValueAt(0);
				shots[shots_counter]->element.curve_info.end_curve = shots[shots_counter]->element.curve_info.curve->curve.ValueAt(1);
				shots[shots_counter]->element.curve_info.start_front = shots[shots_counter]->element.curve_info.cam->frustum.front;

				shots[shots_counter]->element.curve_info.end_front = shots[shots_counter]->element.curve_info.target->transform->GetGlobalPosition() - shots[shots_counter]->element.curve_info.start_curve;
				shots[shots_counter]->element.curve_info.start_pos = shots[shots_counter]->element.curve_info.cam->game_object_attached->transform->GetGlobalPosition();
				shots[shots_counter]->element.first_frame = false;
			
		}

		float3 currentPos = shots[shots_counter]->element.curve_info.target->transform->GetGlobalPosition();
		float3 front_vector = shots[shots_counter]->element.curve_info.cam->frustum.front;

		if (shots[shots_counter]->element.curve_info.start_transition)
		{

			shots[shots_counter]->element.curve_info.current_time_transition += Time::GetDT();
			if (shots[shots_counter]->element.curve_info.current_time_transition >= shots[shots_counter]->element.curve_info.transition_duration)
			{
				shots[shots_counter]->element.curve_info.start_transition = false;
				shots[shots_counter]->element.curve_info.current_time_transition = shots[shots_counter]->element.curve_info.transition_duration;
			}

			float percentatge_lerp = shots[shots_counter]->element.curve_info.current_time_transition / shots[shots_counter]->element.curve_info.transition_duration;
			currentPos = math::Lerp(shots[shots_counter]->element.curve_info.start_pos, shots[shots_counter]->element.curve_info.start_curve, percentatge_lerp);
			front_vector = math::Lerp(shots[shots_counter]->element.curve_info.start_front, shots[shots_counter]->element.curve_info.end_front, percentatge_lerp);
			front_vector.Normalize();
			shots[shots_counter]->element.curve_info.cam->frustum.up = float3::unitY();
		}


		else if (shots[shots_counter]->element.curve_info.go_back)
		{
			shots[shots_counter]->element.curve_info.current_time_transition += Time::GetDT();
			if (shots[shots_counter]->element.curve_info.entered_go_back)
			{
				shots[shots_counter]->element.curve_info.entered_go_back = false;
				shots[shots_counter]->element.curve_info.go_back = false;
				shots[shots_counter]->element.curve_info.ended_intro = true;
				shots[shots_counter]->element.curve_info.front_end = shots[shots_counter]->element.curve_info.cam->frustum.front;
				
			}
			if (shots[shots_counter]->element.curve_info.current_time_transition >= shots[shots_counter]->element.curve_info.transition_duration)
			{
				shots[shots_counter]->element.curve_info.current_time_transition = shots[shots_counter]->element.curve_info.transition_duration;
			
			}


			float percentatge_lerp = shots[shots_counter]->element.curve_info.current_time_transition / shots[shots_counter]->element.curve_info.transition_duration;
			currentPos = math::Lerp(shots[shots_counter]->element.curve_info.end_curve, shots[shots_counter]->element.curve_info.start_pos, percentatge_lerp);
			//front_vector = math::Lerp(shots[shots_counter]->element.curve_info.front_end, shots[shots_counter]->element.curve_info.start_front, percentatge_lerp);
			//front_vector = shots[shots_counter]->element.curve_info.target->transform->GetGlobalPosition() - currentPos;
			//front_vector.Normalize();
		}


		else
		{

			currentPos = shots[shots_counter]->element.curve_info.curve->curve.ValueAt(shots[shots_counter]->element.curve_info.current_pos);
			shots[shots_counter]->element.curve_info.current_pos += shots[shots_counter]->element.curve_info.speed * Time::GetDT();
			if (shots[shots_counter]->element.curve_info.current_pos > 1)
			{
				//shots[shots_counter]->element.curve_info.go_back = true;
				state = CutsceneState::IDLE;
			}
			front_vector = shots[shots_counter]->element.curve_info.target->transform->GetGlobalPosition() - currentPos;
			front_vector.Normalize();
			shots[shots_counter]->element.curve_info.cam->frustum.up = float3::unitY();

		}


		shots[shots_counter]->element.curve_info.cam->frustum.front = front_vector;
		camera->transform->SetGlobalPosition(currentPos);
	}
	else {
		shots[shots_counter]->element.curve_info.current_time_transition = 0;
		

		shots[shots_counter]->element.stay_timer = Time::GetGameTime();
		if (shots[shots_counter]->element.it_shake)
			shots[shots_counter]->element.info_shake.shake_timer = Time::GetGameTime();
	}
}

void CutsceneCamera::BuildCurve(CutsceneShot* shot)
{
	if (shot->element.curve_info.path)
	{
		shot->element.curve_info.curve = shot->element.curve_info.path->GetComponent<ComponentCurve>();
		if (shot->element.curve_info.curve != nullptr)
		{
			LOG("Curve found!");
			shot->element.curve_info.cam = camera->GetComponent<ComponentCamera>();
		}

	}
}

bool CutsceneCamera::OrderCutscenes(CutsceneShot* i, CutsceneShot* j)
{
	return (i->element.id_shot < j->element.id_shot); 
}
