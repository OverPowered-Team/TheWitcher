#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "CameraEnterExit.h"

struct TransitionInfo
{
	TransitionInfo()
	{};

	TransitionInfo(float distance, float hor_angle, float vert_angle, float transition_time)
		: distance(distance), hor_angle(hor_angle), vert_angle(vert_angle), transition_time(transition_time)
	{};

	float distance = 0.f;
	float hor_angle = 0.f;
	float vert_angle = 0.f;
	float transition_time = 2.f;
};

class CameraMovement;


class ALIEN_ENGINE_API TriggerCamera : public Alien {
public:
	TriggerCamera();
	virtual ~TriggerCamera();
	void Start() override;
	void Update() override;//TODO: Remove
	void StartTransition(TransitionInfo transition_info);
	
	void RegisterMovement(int playerNum, int collider_position);
	bool PlayerMovedForward(int player_num);
	bool PlayerMovedBackward(int player_num);
	void VisualizeCameraTransition(TransitionInfo transition_info, Color color);

	void OnDrawGizmos() override;

public:
	std::vector<std::vector<int>> registered_position = { {}, {} };
	//TransitionInfo prev_camera;
	float prev_camera_distance = 0.f;
	float prev_camera_hor_angle = 0.f;
	float prev_camera_vert_angle = 0.f;
	float prev_camera_transition_time = 2.f;
	//TransitionInfo next_camera;
	float next_camera_distance = 0.f;
	float next_camera_hor_angle = 0.f;
	float next_camera_vert_angle = 0.f;
	float next_camera_transition_time = 2.f;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
};

ALIEN_FACTORY TriggerCamera* CreateTriggerCamera() {
	TriggerCamera* alien = new TriggerCamera();
	// To show in inspector here
	SHOW_TEXT("Previous camera");//The camera that will transition to when all players have moved back
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera_distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera_hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera_vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(   alien->prev_camera_transition_time);
	SHOW_SEPARATOR();

	SHOW_TEXT("Next camera");//The camera that will transition to when all players have moved forward
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera_distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera_hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera_vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(   alien->next_camera_transition_time);
	SHOW_SEPARATOR();

	return alien;
}