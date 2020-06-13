#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(SceneChanges,
	TutorialToLvl1,
	BalanToLvl2,
	Lv2ToWagon,
	WagonToCiri,
	ForceTutorial,
	NONE
	);

class ALIEN_ENGINE_API LoadingBackground : public Alien {

public:

	LoadingBackground();
	virtual ~LoadingBackground();
	
	void Start();
	void Update();


	SceneChanges sceneToChange = SceneChanges::NONE;


	ComponentText* text = nullptr;
	ComponentAnimatedImage* anim = nullptr;

	bool lerping = false;

	float time = 0;

	bool change_next = false;
};

ALIEN_FACTORY LoadingBackground* CreateLoadingBackground() {
	LoadingBackground* alien = new LoadingBackground();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(SceneChanges, alien->sceneToChange);
	return alien;
} 
