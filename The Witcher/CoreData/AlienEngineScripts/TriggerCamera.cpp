#include "TriggerCamera.h"
#include "../../Alien Engine/StaticTween.h"
#include "CameraEnterExit.h"
#include "CameraMovement.h"

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
}

void TriggerCamera::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		StartTransition(TransitionInfo(prev_camera_distance, prev_camera_hor_angle, prev_camera_vert_angle, prev_camera_transition_time));
	}
	if (Input::GetKeyDown(SDL_SCANCODE_2)) {
		StartTransition(TransitionInfo(next_camera_distance, next_camera_hor_angle, next_camera_vert_angle, next_camera_transition_time));
	}
}

void TriggerCamera::StartTransition(TransitionInfo transition_info)
{
	if (cam_script != nullptr) {
		cam_script->current_transition_time = 0.f;
		cam_script->state = CameraMovement::CameraState::MOVING_TO_DYNAMIC;
		cam_script->curr_transition = transition_info;
		cam_script->trg_offset = cam_script->CalculateCameraPos(transition_info.hor_angle, transition_info.vert_angle, transition_info.distance);
		//cam_script->start_transition_pos = camera->transform->GetGlobalPosition();
		LOG("Started transition");
	}
}

void TriggerCamera::OnDrawGizmos()
{
	GameObject* enter = game_object->GetChild("Enter");
	GameObject* exit = game_object->GetChild("Exit");
	if (enter != nullptr && exit != nullptr) {
		Gizmos::DrawLine(enter->transform->GetGlobalPosition(), exit->transform->GetGlobalPosition(), Color::Red());
	}

	if (cam_script == nullptr) {
		camera = Camera::GetCurrentCamera()->game_object_attached;
		cam_script = (CameraMovement*)camera->GetComponentScript("CameraMovement");
	}
	VisualizeCameraTransition(TransitionInfo(prev_camera_distance, prev_camera_hor_angle, prev_camera_vert_angle, prev_camera_transition_time), Color::Red());
	VisualizeCameraTransition(TransitionInfo(next_camera_distance, next_camera_hor_angle, next_camera_vert_angle, next_camera_transition_time), Color::Green());
}

void TriggerCamera::VisualizeCameraTransition (TransitionInfo transition_info, Color color) {
	float3 camera_pos = transform->GetGlobalPosition() + cam_script->CalculateCameraPos(transition_info.hor_angle, transition_info.vert_angle, transition_info.distance);
	Gizmos::DrawLine(transform->GetGlobalPosition(), camera_pos, color); // line mid -> future camera pos
	Gizmos::DrawSphere(camera_pos, 0.15f, color);
}

void TriggerCamera::RegisterMovement(int player_num, int collider_position)
{
	//TODO: Remove this workaround when we fix the bug that collider enters 2 times on OnTriggerEnter
	//INFO: This is done to avoid duplicates
	int size = registered_position[player_num].size();
	if (!(size > 0
		&& registered_position[player_num][size - 1] == collider_position))
	{
		LOG("entered collider: %i", collider_position);
		registered_position[player_num].push_back(collider_position);

		//TODO: Create a for that checks for all players
		if (PlayerMovedForward(0) && PlayerMovedForward(1)) {
			LOG("All players moved forward - Transition to next camera");
			StartTransition(TransitionInfo(next_camera_distance, next_camera_hor_angle, next_camera_vert_angle, next_camera_transition_time));
		}
		if (PlayerMovedBackward(0) && PlayerMovedBackward(1)) {
			LOG("All players moved back - Transition to prev camera");
			StartTransition(TransitionInfo(prev_camera_distance, prev_camera_hor_angle, prev_camera_vert_angle, prev_camera_transition_time));
		}
	}
}

bool TriggerCamera::PlayerMovedForward(int player_num) {
	int size = registered_position[player_num].size();
	if (size < 2) {
		return false;
	}
	return (registered_position[player_num][size - 2] == 1
		&& registered_position[player_num][size - 1] == 2);
}

bool TriggerCamera::PlayerMovedBackward(int player_num) {
	int size = registered_position[player_num].size();
	if (size < 2) {
		return false;
	}
	return (registered_position[player_num][size - 2] == 1
		&& registered_position[player_num][size - 1] == 0);
}
