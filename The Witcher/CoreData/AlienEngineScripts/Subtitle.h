#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "..\..\ComponentText.h"
#include "..\..\Timer.h"
#include <vector>

class subtitle
{
public:
	float start = 0, end = 0;
	std::string text;
};
class ALIEN_ENGINE_API Subtitle : public Alien {

public:

	Subtitle();
	virtual ~Subtitle();
	
	void Start();
	void Update();

	std::vector<subtitle> subtitles;
	double current_time = 0;
	int current_sub = 0;
	ComponentText* text = nullptr;
	ComponentAudioEmitter* audio = nullptr;
	ComponentAudioEmitter* song = nullptr;
	GameObject* songBard = nullptr;
	bool first_entered = true;
	float end_seconds = 69;
	float start_time;
	bool change_scene = false;

};

ALIEN_FACTORY Subtitle* CreateSubtitle() {
	Subtitle* alien = new Subtitle();
	
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->songBard, "Song gameobject");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->end_seconds);
	// To show in inspector here
	return alien;
} 
