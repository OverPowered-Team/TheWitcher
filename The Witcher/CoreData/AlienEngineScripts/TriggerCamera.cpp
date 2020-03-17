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
	camera = Camera::GetCurrentCamera()->game_object_attached;
	cam_script = (CameraMovement*)camera->GetComponentScript("CameraMovement");
	t1 = Time::GetGameTime();
}

void TriggerCamera::Update()
{
	if (info_to_cam.tp_players && cam_script != nullptr && player_counter == 1)
	{
		ManageTransition();
		TeleportTheRestOfPlayers();
	}
	else if (cam_script != nullptr && player_counter == cam_script->num_curr_players)
	{
		ManageTransition();
		
	}
}

void TriggerCamera::ManageTransition()
{
	if (IsCameraDifferent())
	{
		if (state == ToState::DYNAMIC) {
			cam_script->destination = cam_script->CalculateCameraPos(info_to_cam.hor_angle, info_to_cam.vert_angle, info_to_cam.distance);
			cam_script->state = CameraMovement::CameraState::MOVING_TO_DYNAMIC;
		}
		else if (state == ToState::STATIC) {
			cam_script->destination = static_pos->transform->GetGlobalPosition();
			cam_script->state = CameraMovement::CameraState::MOVING_TO_STATIC;
		}

		InterChangeInfoWithCamera();
		Tween::TweenMoveTo(camera, cam_script->destination, 2, Tween::linear);
		cam_script->t1 = Time::GetGameTime();
	}
	player_counter = 0;
}

void TriggerCamera::OnDrawGizmos()
{
	GameObject* enter = game_object->GetChild("Enter");
	GameObject* exit = game_object->GetChild("Exit");
	if (enter != nullptr && exit != nullptr) {
		Gizmos::DrawLine(enter->transform->GetGlobalPosition(), exit->transform->GetGlobalPosition(), Color::Red());
		Gizmos::DrawSphere(exit->transform->GetGlobalPosition(), 0.3f, Color::Red());
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

void TriggerCamera::TeleportTheRestOfPlayers()
{
	float3 first_player_pos = info_to_cam.first_player->transform->GetGlobalPosition();
	GameObject** get_players = nullptr;
	uint size;
	size = GameObject::FindGameObjectsWithTag("Player", &get_players);
	for (int i = 0; i < size; i++) {
		GameObject* g = get_players[i];
		if (g != info_to_cam.first_player)
		{
			float3 dist(info_to_cam.tp_distance, 0, 0); //Delete this info?
			g->transform->SetGlobalPosition(first_player_pos + dist);
		}
	}
	info_to_cam.first_player = nullptr;
}

bool TriggerCamera::IsCameraDifferent()
{
	if (info_to_cam.hor_angle == cam_script->top_angle &&
		info_to_cam.vert_angle == cam_script->vertical_angle &&
		info_to_cam.distance == cam_script->distance)
	{
		return false;
	}
	return true;
}
