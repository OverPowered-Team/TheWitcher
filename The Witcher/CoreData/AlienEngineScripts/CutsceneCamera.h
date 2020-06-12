#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CutsceneShot;
class CameraMovement;
class CameraShake;

class ALIEN_ENGINE_API CutsceneCamera : public Alien {

public:

	enum(CutsceneState,
		IDLE, MOVING);



	CutsceneCamera();
	virtual ~CutsceneCamera();
	
	void Start();
	void Update();
	void CleanUp() override;
	void BuildCutscene();
	void PrepareCutscene();
	void ExecuteCutscene();
	void ExecuteCurve();
	void BuildCurve(CutsceneShot* shot);
	static bool OrderCutscenes(CutsceneShot* i, CutsceneShot* j);
public:
	vector<CutsceneShot*> shots;
	GameObject* camera = nullptr;
	CameraMovement* cam_movement = nullptr;
	CameraShake* cam_shaking = nullptr;
	int shots_counter = 0;
	CutsceneState state = CutsceneState::IDLE;
	float t_speed = 0.f;
	float internal_timer = 0.f;
};

ALIEN_FACTORY CutsceneCamera* CreateCutsceneCamera() {
	CutsceneCamera* alien = new CutsceneCamera();
	// To show in inspector here

	return alien;
} 
