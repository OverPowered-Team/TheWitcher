#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "CameraMovement.h"

class ALIEN_ENGINE_API TriggerCamera : public Alien {
public:
	struct InfoCamera
	{
		GameObject* first_player = nullptr;
		float distance = 0.f;
		float hor_angle = 0.f;
		float vert_angle = 0.f;
		bool tp_players = false;
		float tp_distance = 0.f;
	};
	TriggerCamera();
	virtual ~TriggerCamera();
	void Start();
	void Update();
	void ManageTransition();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);
	void InterChangeInfoWithCamera();
	void TeleportTheRestOfPlayers();
	bool IsCameraDifferent();

public:
	InfoCamera info_to_cam;
	InfoCamera info_from_cam;
	int player_counter = 0;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	bool ftime = true;
	bool is_backing = false;
	float t1 = 0.f;
};
ALIEN_FACTORY TriggerCamera* CreateTriggerCamera() {
	TriggerCamera* alien = new TriggerCamera();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->camera);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.distance);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.hor_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.vert_angle, -360.f, 360.f);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->info_to_cam.tp_players);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->info_to_cam.tp_distance);
	return alien;
}