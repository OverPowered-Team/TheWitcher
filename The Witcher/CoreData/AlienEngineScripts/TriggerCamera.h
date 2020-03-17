#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "CameraMovement.h"

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
	void InterChangeInfoWithCamera();
	bool IsCameraDifferent();

	void OnDrawGizmos() override;

public:
	InfoCamera info_to_cam;
	InfoCamera info_from_cam;
	int player_counter = 0;
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

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.vert_angle, -360.f, 360.f);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->static_pos);
	
	return alien;
}