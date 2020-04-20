#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "TriggerCamera.h"

class ALIEN_ENGINE_API CameraMovement : public Alien {
public:
	//enum class CameraState {
	//	NONE = -1,
	//	DYNAMIC, STATIC, MOVING, AXIS,
	//	MAX
	//};
	enum (CameraState,
		DYNAMIC, MOVING_TO_DYNAMIC,
		STATIC, MOVING_TO_STATIC,
		AXIS, MOVING_TO_AXIS
		);
	enum (
		CameraAxis,
		NONE,X,Y,Z
	);

	CameraMovement();
	virtual ~CameraMovement();
	
	void Start();
	void Update();
	void OnDrawGizmos();
	void SearchAndAssignPlayers();
	float3 CalculateMidPoint();
	float3 CalculateAxisMidPoint();
	void LookAtMidPoint();
	float3 CalculateCameraPos(const float& vertical, const float& top_view, const float& dst);
	Quat RotationBetweenVectors(math::float3& front, math::float3& direction);

	TransitionInfo curr_transition;
	CameraState state = CameraState::DYNAMIC;
	std::vector<GameObject*> players;
	uint num_curr_players = 0u;
	float3 trg_offset;
	CameraAxis axis = CameraAxis::NONE;
	float current_transition_time = 0.f;

	float distance = 0.f;
	float hor_angle = 0.f;
	float vert_angle = 0.f;

	bool smooth_camera = true;
	float smooth_cam_vel = 5.f;
};

ALIEN_FACTORY CameraMovement* CreateCameraMovement() {
	CameraMovement* alien = new CameraMovement();
	// To show in inspector here
	SHOW_TEXT("First parameters");
	SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraState, alien->state);

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->distance, 0, 100);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraAxis, alien->axis);

	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->smooth_camera);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->smooth_cam_vel);

	return alien;
}
