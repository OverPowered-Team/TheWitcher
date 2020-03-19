#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "CameraMovement.h"
#include "CameraEnterExit.h"

class ALIEN_ENGINE_API TriggerCamera : public Alien {
public:

	enum (ToState,
		DYNAMIC,
		STATIC,
		AXIS_NOT_IMPLEMENTED
	);

	struct InfoCamera
	{
		float distance = 0.f;
		float hor_angle = 0.f;
		float vert_angle = 0.f;
	};

	TriggerCamera();
	virtual ~TriggerCamera();
	void Start();
	void Update();
	void ManageTransition(bool normal_way);
	void InterChangeInfoWithCamera();
	bool IsCameraDifferent();
	
	void RegisterMovement(int playerNum, int collider_position);
	bool PlayerMovedForward(int player_num);
	bool PlayerMovedBackward(int player_num);

	void OnDrawGizmos() override;

public:
	std::vector<std::vector<int>> registered_position = { {}, {} };
	InfoCamera prev_camera;
	InfoCamera next_camera;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	GameObject* static_pos = nullptr;
	ToState state = ToState::DYNAMIC;
	bool ftime = true;
	bool is_backing = false;
	float t1 = 0.f;
};

ALIEN_FACTORY TriggerCamera* CreateTriggerCamera() {
	TriggerCamera* alien = new TriggerCamera();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerCamera::ToState, alien->state);
	SHOW_SEPARATOR();

	SHOW_TEXT("Dynamic");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->prev_camera.vert_angle, -360.f, 360.f);
	SHOW_SEPARATOR();
	SHOW_TEXT("Static");
	//SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->info_to_cam.tp_players);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->static_pos);
	return alien;
}