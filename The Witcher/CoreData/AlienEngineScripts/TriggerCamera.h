#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "CameraEnterExit.h"

struct TransitionInfo
{
	enum (ToTransitionType,
		DYNAMIC, STATIC
		);

	TransitionInfo()
	{};

	TransitionInfo(float distance, float hor_angle, float vert_angle, float transition_time)
		: distance(distance), hor_angle(hor_angle), vert_angle(vert_angle), transition_time(transition_time), type(ToTransitionType::DYNAMIC)
	{};

	TransitionInfo(GameObject* to_move, float transition_time)
		: to_move(to_move), transition_time(transition_time), type(ToTransitionType::STATIC)
	{}

	float distance;
	float hor_angle;
	float vert_angle;

	GameObject* to_move = nullptr;

	float transition_time = 2.f;

	TransitionInfo::ToTransitionType type = TransitionInfo::ToTransitionType::DYNAMIC;
};

class CameraMovement;

class ALIEN_ENGINE_API TriggerCamera : public Alien {
public:
	
	TriggerCamera();
	virtual ~TriggerCamera();
	void Start() override;
	void StartTransition(TransitionInfo transition_info);
	
	void RegisterMovement(int playerNum, int collider_position);
	bool PlayerMovedForward(int player_num);
	bool PlayerMovedBackward(int player_num);
	void VisualizeCameraTransition(TransitionInfo transition_info, Color color);

	void OnDrawGizmos() override;

public:
	std::vector<std::vector<int>> registered_position = { {}, {} };
	TransitionInfo prev_camera;
	TransitionInfo next_camera;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
};

ALIEN_FACTORY TriggerCamera* CreateTriggerCamera() {
	TriggerCamera* alien = new TriggerCamera();
	// To show in inspector here
	SHOW_TEXT("Previous camera");//The camera that will transition to when all players have moved back
	SHOW_IN_INSPECTOR_AS_ENUM(TransitionInfo::ToTransitionType, alien->prev_camera.type);
	SHOW_TEXT("If Dynamic");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->prev_camera.transition_time);
	SHOW_TEXT("If Static");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->prev_camera.to_move);
	SHOW_SEPARATOR();

	SHOW_TEXT("Next camera");//The camera that will transition to when all players have moved forward
	SHOW_IN_INSPECTOR_AS_ENUM(TransitionInfo::ToTransitionType, alien->next_camera.type);
	SHOW_TEXT("If Dynamic");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera.distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera.hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->next_camera.vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->next_camera.transition_time);
	SHOW_TEXT("If Static");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->next_camera.to_move);
	SHOW_SEPARATOR();

	return alien;
}