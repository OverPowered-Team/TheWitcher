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
		info = &info_to_cam;
	else
		info = &info_from_cam;

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
		float3 cam_pos = transform->GetGlobalPosition() + cam_script->CalculateCameraPos(info_to_cam.hor_angle, info_to_cam.vert_angle, info_to_cam.distance, false);
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
	// TODO: update to new STATIC MODE
	info_from_cam.hor_angle = cam_script->top_angle;
	info_from_cam.vert_angle = cam_script->vertical_angle;
	info_from_cam.distance = cam_script->distance;
	/*if (ftime)
	{*/
	/*ftime = false;
	}
	cam_script->top_angle = info_to_cam.hor_angle;
	cam_script->vertical_angle = info_to_cam.vert_angle;
	cam_script->distance = info_to_cam.distance;*/
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
