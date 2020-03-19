#include "TriggerCamera.h"
#include "../../Alien Engine/StaticTween.h"
#include "CameraEnterExit.h"

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
	/*if (info_to_cam.tp_players && cam_script != nullptr && player_counter == 1)
	{
		ManageTransition();
		TeleportTheRestOfPlayers();
	}
	else if (cam_script != nullptr && player_counter == cam_script->num_curr_players)
	{
		ManageTransition();
	}*/
}

void TriggerCamera::ManageTransition(bool normal_way)
{
	/*if (IsCameraDifferent())
	{*/
	InfoCamera* info = nullptr;
	if (normal_way)
		info = &prev_camera;
	else
		info = &next_camera;

	if (state == ToState::DYNAMIC) {
		if (cam_script != nullptr) {
			cam_script->destination = cam_script->CalculateCameraPos(info->hor_angle, info->vert_angle, info->distance);
			cam_script->state = CameraMovement::CameraState::MOVING_TO_DYNAMIC;
		}
		else {
			LOG("THERE IS NOT CAMERA MOVEMENT SCRIPT IN CURRENT CAMERA");
		}
	}
	else if (state == ToState::STATIC) {
		if (static_pos != nullptr) {
			cam_script->destination = static_pos->transform->GetGlobalPosition();
			cam_script->state = CameraMovement::CameraState::MOVING_TO_STATIC;
		}
		else {
			LOG("Static object is NULL");
		}
	}
	if (cam_script != nullptr) {
		cam_script->current_transition_time = 0.f;
		cam_script->state = CameraMovement::CameraState::MOVING_TO_DYNAMIC;
		cam_script->top_angle = info->hor_angle;
		cam_script->vertical_angle = info->vert_angle;
		cam_script->distance = info->distance;
		//InterChangeInfoWithCamera();
		//Tween::TweenMoveTo(camera, cam_script->destination, 2, Tween::linear);
		//cam_script->t1 = Time::GetGameTime();
	}
	/*}
	player_counter = 0;*/
}

void TriggerCamera::OnDrawGizmos()
{
	GameObject* enter = game_object->GetChild("Enter");
	GameObject* exit = game_object->GetChild("Exit");
	if (enter != nullptr && exit != nullptr) {
		Gizmos::DrawLine(enter->transform->GetGlobalPosition(), exit->transform->GetGlobalPosition(), Color::Red());
		Gizmos::DrawSphere(exit->transform->GetGlobalPosition(), 0.3f, Color::Red());
	}

	switch (state)
	{
	case TriggerCamera::ToState::DYNAMIC: {
		if (cam_script == nullptr) {
			camera = Camera::GetCurrentCamera()->game_object_attached;
			cam_script = (CameraMovement*)camera->GetComponentScript("CameraMovement");
		}
		float3 cam_pos = transform->GetGlobalPosition() + cam_script->CalculateCameraPos(prev_camera.hor_angle, prev_camera.vert_angle, prev_camera.distance, false);
		Gizmos::DrawLine(transform->GetGlobalPosition(), cam_pos, Color::Red()); // line mid -> future camera pos
		Gizmos::DrawSphere(cam_pos, 0.15f, Color::Green());
		break;
	}
	case TriggerCamera::ToState::STATIC: {
		if (static_pos != nullptr)
			Gizmos::DrawSphere(static_pos->transform->GetGlobalPosition(), 0.5, Color::Purple());
		break;
	}
	case TriggerCamera::ToState::AXIS_NOT_IMPLEMENTED:
		break;
	default:
		break;
	}

	
	//Gizmos::DrawLine(cam_pos, cam_pos + axis_cam.Normalized(), Color::Magenta());
}

void TriggerCamera::InterChangeInfoWithCamera()
{
	next_camera.hor_angle = cam_script->top_angle;
	next_camera.vert_angle = cam_script->vertical_angle;
	next_camera.distance = cam_script->distance;
}

bool TriggerCamera::IsCameraDifferent()
{
	if (prev_camera.hor_angle == cam_script->top_angle &&
		prev_camera.vert_angle == cam_script->vertical_angle &&
		prev_camera.distance == cam_script->distance)
	{
		return false;
	}
	return true;
}

void TriggerCamera::RegisterMovement(int playerNum, int collider_position)
{
	//TODO: Remove this workaround when we fix the bug that collider enters 2 times on OnTriggerEnter
	//INFO: This is done to avoid duplicates
	int size = registered_position[playerNum].size();
	if (!(size > 0
		&& registered_position[playerNum][size - 1] == collider_position))
	{
		LOG("entered collider: %i", collider_position);
		registered_position[playerNum].push_back(collider_position);

		//TODO: Create a for that checks for all players
		if (PlayerMovedForward(0)/* && PlayerMovedForward(1)*/) {
			LOG("All players forward");
		}
		if (PlayerMovedBackward(0) /*&& PlayerMovedBackward(1)*/) {
			LOG("All players back");
		}
	}
}

bool TriggerCamera::PlayerMovedForward(int player_num) {
	int size = registered_position[player_num].size();
	if (size < 2) {
		return false;
	}
	return (registered_position[player_num][size - 2] == 0
		&& registered_position[player_num][size - 1] == 1);
}

bool TriggerCamera::PlayerMovedBackward(int player_num) {
	int size = registered_position[player_num].size();
	if (size < 2) {
		return false;
	}
	return (registered_position[player_num][size - 2] == 1
		&& registered_position[player_num][size - 1] == 0);
}
