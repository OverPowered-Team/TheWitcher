#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CutsceneShot;
class CameraMovement;

class ALIEN_ENGINE_API CutsceneCamera : public Alien {

public:

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
};

ALIEN_FACTORY CutsceneCamera* CreateCutsceneCamera() {
	CutsceneCamera* alien = new CutsceneCamera();
	// To show in inspector here

	return alien;
} 
