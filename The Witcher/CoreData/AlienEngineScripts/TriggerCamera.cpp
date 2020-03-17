#include "TriggerCamera.h"
#include "../../Alien Engine/StaticTween.h"

TriggerCamera::TriggerCamera() : Alien()
{
}

TriggerCamera::~TriggerCamera()
{
}

void TriggerCamera::Start()
{
	cam_script = (CameraMovement*)camera->GetComponentScript("CameraMovement");
	t1 = Time::GetGameTime();
	
}

void TriggerCamera::Update()
{
	if (cam_script != nullptr && player_counter == cam_script->num_curr_players)
	{
		//bool yes = ;
		//if (yes)
		//	cam_script->destination = cam_script->CalculateCameraPos(info_from_cam.hor_angle, info_from_cam.vert_angle, info_from_cam.distance);
		//else
		if (IsCameraDifferent())
		{
			cam_script->destination = cam_script->CalculateCameraPos(info_to_cam.hor_angle, info_to_cam.vert_angle, info_to_cam.distance);
			InterChangeInfoWithCamera();
			Tween::TweenMoveTo(camera, cam_script->destination, 2, Tween::linear);
			cam_script->t1 = Time::GetGameTime();
			cam_script->state = CameraMovement::CameraState::MOVING;
		}
		player_counter = 0;
	}
}

void TriggerCamera::OnTriggerEnter(ComponentCollider* collider)
{
	Component* c = (Component*)collider;

	if (player_counter < cam_script->num_curr_players + 1)
	{
		for (std::map<GameObject*, bool>::iterator it = cam_script->players.begin(); it != cam_script->players.end(); ++it)
		{
			if (it->first == c->game_object_attached && it->second == false)
			{
				it->second = true;
				player_counter++;
				LOG("PLAYER ENTER");
			}
					
		}
	}
}

void TriggerCamera::OnTriggerExit(ComponentCollider* collider)
{
	Component* c = (Component*)collider;
	if (player_counter >= 0)
	{
		if (player_counter == 0)
			player_counter = cam_script->num_curr_players;
		for (std::map<GameObject*, bool>::iterator it = cam_script->players.begin(); it != cam_script->players.end(); ++it)
		{
			if (it->first == c->game_object_attached)
			{
				it->second = false;
				player_counter--;
				LOG("PLAYER EXIT");
			}

		}
	}
}

void TriggerCamera::InterChangeInfoWithCamera()
{
	if (ftime)
	{
		info_from_cam.hor_angle = cam_script->top_angle;
		info_from_cam.vert_angle = cam_script->vertical_angle;
		info_from_cam.distance = cam_script->distance;
		ftime = false;
	}
	cam_script->top_angle = info_to_cam.hor_angle;
	cam_script->vertical_angle = info_to_cam.vert_angle;
	cam_script->distance = info_to_cam.distance;
}

bool TriggerCamera::IsCameraDifferent()
{
	//if (is_backing)
	//{
	//	cam_script->top_angle = info_from_cam.hor_angle;
	//	cam_script->vertical_angle = info_from_cam.vert_angle;
	//	cam_script->distance = info_from_cam.distance;
	//	is_backing = false;
	//}
	if (info_to_cam.hor_angle == cam_script->top_angle &&
		info_to_cam.vert_angle == cam_script->vertical_angle &&
		info_to_cam.distance == cam_script->distance)
	{
		//is_backing = true;
		return false;
	}
	return true;
}
