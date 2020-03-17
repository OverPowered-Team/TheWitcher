#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraMovement : public Alien {
public:
	//enum class CameraState {
	//	NONE = -1,
	//	DYNAMIC, STATIC, MOVING, AXIS,
	//	MAX
	//};
	enum (CameraState,
		DYNAMIC, STATIC, MOVING_TO_STATIC, MOVING_TO_DYNAMIC, AXIS,
		);
	enum (
		CameraAxis,
		X,Y,Z
	);
	enum (PlayerState,
		NONE, ENTER_FIRST, ENTER_FIRST_MIDDLE, ENTER_LAST, EXIT_FIRST, EXIT_FIRST_MIDDLE, EXIT_LAST, ON_EXIT
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

	float top_angle = 0.f;
	float vertical_angle = 0.f;
	float distance = 5.f;
	CameraState state = CameraState::DYNAMIC;
	float3 diff_pos;
	float3 destination;
	float t1 = 0.f;
	CameraAxis axis = CameraAxis::X;
	std::map<GameObject*, PlayerState> players;
	uint num_curr_players = 0u;
	float3 axis_cam;

};

ALIEN_FACTORY CameraMovement* CreateCameraMovement() {
	CameraMovement* alien = new CameraMovement();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->top_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->vertical_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->distance);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.x,0, 1);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.y, 0, 1);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.z, 0, 1);
	SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraState, alien->state);
	SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraAxis, alien->axis);

	return alien;
}
