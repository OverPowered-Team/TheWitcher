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
	cam_script = camera->GetComponent<CameraMovement>();
}

void TriggerCamera::StartTransition(TransitionInfo& transition_info)
{
	if (cam_script != nullptr) {
		cam_script->current_transition_time = 0.f;
		switch (transition_info.type)
		{
		case TransitionInfo::ToTransitionType::DYNAMIC:
			cam_script->state = CameraMovement::CameraState::MOVING_TO_DYNAMIC;
			cam_script->trg_offset = cam_script->CalculateCameraPos(transition_info.hor_angle, transition_info.vert_angle, transition_info.distance);
			break;
		case TransitionInfo::ToTransitionType::STATIC:
			cam_script->state = CameraMovement::CameraState::MOVING_TO_STATIC;
			cam_script->trg_offset = transition_info.to_move->transform->GetGlobalPosition();
			break;
		case TransitionInfo::ToTransitionType::AXIS:
			cam_script->state = CameraMovement::CameraState::MOVING_TO_AXIS;
			cam_script->axis = (CameraMovement::CameraAxis)transition_info.axis_type;
			cam_script->trg_offset = transition_info.to_move->transform->GetGlobalPosition();		
			break;
		default:
			break;
		}
		cam_script->curr_transition = transition_info;
		cam_script->hor_angle = transition_info.hor_angle;
		cam_script->vert_angle = transition_info.vert_angle;
		cam_script->distance = transition_info.distance;
		cam_script->is_cinematic = transition_info.is_cinematic;
		cam_script->transition_time = transition_info.transition_time;
		cam_script->cutscene_game_object = transition_info.cutscene;

		if (next_camera.is_cinematic) //Cutscenes only need to play themselves the first time, I know this is bad, forgive me for my sins
			next_camera.is_cinematic = false;

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
		cam_script = camera->GetComponent<CameraMovement>();
	}

	switch (prev_camera.type)
	{
	case TransitionInfo::ToTransitionType::DYNAMIC: {
		TransitionInfo transition = TransitionInfo(prev_camera.distance, prev_camera.hor_angle, prev_camera.vert_angle, prev_camera.transition_time, prev_camera.is_cinematic, next_camera.cutscene);
		VisualizeCameraTransition(transition, Color::Red());
		break;
	}
	case TransitionInfo::ToTransitionType::STATIC:
		VisualizeCameraTransition(TransitionInfo(prev_camera.to_move, prev_camera.transition_time, prev_camera.is_cinematic, next_camera.cutscene), Color::Green());
		break;
	case TransitionInfo::ToTransitionType::AXIS:
		VisualizeCameraTransition(TransitionInfo(prev_camera.to_move, prev_camera.transition_time, prev_camera.axis_type, prev_camera.is_cinematic, next_camera.cutscene), Color::Green());
		break;
		/*case TransitionInfo::ToTransitionType::AXIS:
			VisualizeCameraTransition(TransitionInfo(prev_camera.distance, prev_camera.hor_angle, prev_camera.vert_angle, prev_camera.transition_time, prev_camera.axis_type), Color::Green());
			break;*/
	}

	switch (next_camera.type)
	{
	case TransitionInfo::ToTransitionType::DYNAMIC: {
		TransitionInfo transition = TransitionInfo(next_camera.distance, next_camera.hor_angle, next_camera.vert_angle, next_camera.transition_time, next_camera.is_cinematic, next_camera.cutscene);
		VisualizeCameraTransition(transition, Color::Green());
		break;
	}
	case TransitionInfo::ToTransitionType::STATIC:
		VisualizeCameraTransition(TransitionInfo(next_camera.to_move, next_camera.transition_time, next_camera.is_cinematic, next_camera.cutscene), Color::Green());
		break;
	case TransitionInfo::ToTransitionType::AXIS:
		VisualizeCameraTransition(TransitionInfo(next_camera.to_move, next_camera.transition_time, next_camera.axis_type, next_camera.is_cinematic, next_camera.cutscene), Color::Green());
		break;
	}
}

void TriggerCamera::VisualizeCameraTransition(const TransitionInfo& transition_info, Color color) {
	switch (transition_info.type)
	{
	case TransitionInfo::ToTransitionType::DYNAMIC: {
		float3 camera_pos = transform->GetGlobalPosition() + cam_script->CalculateCameraPos(transition_info.hor_angle, transition_info.vert_angle, transition_info.distance);
		Gizmos::DrawLine(transform->GetGlobalPosition(), camera_pos, color); // line mid -> future camera pos
		Gizmos::DrawSphere(camera_pos, 0.15f, color);
		break;
	}
	case TransitionInfo::ToTransitionType::AXIS:
	case TransitionInfo::ToTransitionType::STATIC:
		Gizmos::DrawSphere(transition_info.to_move->transform->GetGlobalPosition(), 0.3f, Color::Purple());
		Gizmos::DrawLine(transform->GetGlobalPosition(), transition_info.to_move->transform->GetGlobalPosition(), Color::Black());
		break;
	/* AXIS NOW USES EMPTY GAMEOBJECT TO START MOVEMENT
	case TransitionInfo::ToTransitionType::AXIS: {
		float3 camera_pos = transform->GetGlobalPosition() + cam_script->CalculateCameraPos(transition_info.hor_angle, transition_info.vert_angle, transition_info.distance);
		Gizmos::DrawLine(transform->GetGlobalPosition(), camera_pos, color); // line mid -> future camera pos
		Gizmos::DrawSphere(camera_pos, 0.15f, color);
		break;
	}*/
	default:
		break;
	}
}

bool TriggerCamera::AllPlayersMovedForward()
{
	int is_in = 0;
	for (int i = 0; i < cam_script->players.size(); ++i) {
		int size = registered_position[i].size();
		if (size < 2) {
			return false;
		}
		if (registered_position[i][size - 2] == 1
			&& registered_position[i][size - 1] == 2) {
			LOG("FRONT");
			++is_in;
		}
	}
	if (is_in == cam_script->players.size())
		return true;

	return false;
}

bool TriggerCamera::AllPlayersMovedBackward()
{
	int is_in = 0;
	for (int i = 0; i < cam_script->players.size(); ++i) {
		int size = registered_position[i].size();
		if (size < 2) {
			return false;
		}
		if (registered_position[i][size - 2] == 1
			&& registered_position[i][size - 1] == 0) {
			LOG("BACK");
			++is_in;
		}
	}
	if (is_in == cam_script->players.size())
		return true;

	return false;
}

void TriggerCamera::RegisterMovement(int player_num, int collider_position)
{
	//TODO: Remove this workaround when we fix the bug that collider enters 2 times on OnTriggerEnter
	//INFO: This is done to avoid duplicates
	//int size = registered_position[player_num].size();
	//if (!(size > 0
	//	&& registered_position[player_num][size - 1] == collider_position))
	//{
		LOG("entered collider: %i", collider_position);
		registered_position[player_num].push_back(collider_position);

		//TODO: Create a for that checks for all players
		if (AllPlayersMovedForward()) {
			LOG("All players moved forward - Transition to next camera");
			switch (next_camera.type)
			{
			case TransitionInfo::ToTransitionType::DYNAMIC: {
				TransitionInfo info = TransitionInfo(next_camera.distance, next_camera.hor_angle, next_camera.vert_angle, next_camera.transition_time, next_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			case TransitionInfo::ToTransitionType::STATIC: {
				TransitionInfo info = TransitionInfo(next_camera.to_move, next_camera.transition_time, next_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			case TransitionInfo::ToTransitionType::AXIS: {
				TransitionInfo info = TransitionInfo(next_camera.to_move, next_camera.transition_time, next_camera.axis_type, next_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			/*case TransitionInfo::ToTransitionType::AXIS:
				StartTransition(TransitionInfo(next_camera.distance, next_camera.hor_angle, next_camera.vert_angle, next_camera.transition_time, next_camera.axis_type));
				break;*/
			default:
				break;
			}
		}
		if (AllPlayersMovedBackward()) {
			LOG("All players moved back - Transition to prev camera");
			switch (prev_camera.type)
			{
			case TransitionInfo::ToTransitionType::DYNAMIC: {
				TransitionInfo info = TransitionInfo(prev_camera.distance, prev_camera.hor_angle, prev_camera.vert_angle, prev_camera.transition_time, prev_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			case TransitionInfo::ToTransitionType::STATIC: {
				TransitionInfo info = TransitionInfo(prev_camera.to_move, prev_camera.transition_time, prev_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			case TransitionInfo::ToTransitionType::AXIS: {
				TransitionInfo info = TransitionInfo(prev_camera.to_move, prev_camera.transition_time, prev_camera.axis_type, prev_camera.is_cinematic, next_camera.cutscene);
				StartTransition(info);
				break;
			}
			/*case TransitionInfo::ToTransitionType::AXIS:
				StartTransition(TransitionInfo(prev_camera.distance, prev_camera.hor_angle, prev_camera.vert_angle, prev_camera.transition_time, prev_camera.axis_type));
				break;*/
			default:
				break;
			}
		}
	//}
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
