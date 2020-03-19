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
		DYNAMIC, MOVING_TO_DYNAMIC
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
	void LookAtMidPoint();
	float3 CalculateCameraPos(const float& vertical, const float& top_view, const float& dst);
	Quat RotationBetweenVectors(math::float3& front, math::float3& direction);

	TransitionInfo curr_transition;
	CameraState state = CameraState::DYNAMIC;
	std::map<GameObject*, PlayerState> players;
	uint num_curr_players = 0u;
	float3 trg_offset;

	float current_transition_time = 0.f;
};

ALIEN_FACTORY CameraMovement* CreateCameraMovement() {
	CameraMovement* alien = new CameraMovement();
	// To show in inspector here
	SHOW_TEXT("First parameters");
	SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraState, alien->state);
	/*SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.x,0, 1);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.y, 0, 1);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->axis_cam.z, 0, 1);*/
	//SHOW_IN_INSPECTOR_AS_ENUM(CameraMovement::CameraAxis, alien->axis);

	return alien;
}
