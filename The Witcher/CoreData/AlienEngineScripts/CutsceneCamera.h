#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CutsceneShot;
class CameraMovement;


class ALIEN_ENGINE_API CutsceneCamera : public Alien {

public:

	enum(CutsceneState,
		IDLE, SHAKING, MOVING, LOOKING);



	CutsceneCamera();
	virtual ~CutsceneCamera();
	
	void Start();
	void Update();
	void CleanUp() override;
	void BuildCutscene();
	void PrepareCutscene();
	void ExecuteCutscene();
	bool myfunction(int i, int j) { return (i < j); }
public:
	vector<CutsceneShot*> shots;
	CameraMovement* cam_movement = nullptr;
	int shots_counter = 0;
	CutsceneState state = CutsceneState::IDLE;
	float current_move_time = 0.f;
};

ALIEN_FACTORY CutsceneCamera* CreateCutsceneCamera() {
	CutsceneCamera* alien = new CutsceneCamera();
	// To show in inspector here

	return alien;
} 
