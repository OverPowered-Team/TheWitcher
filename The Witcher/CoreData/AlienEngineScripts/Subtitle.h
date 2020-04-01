#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "..\..\ComponentText.h"

#include <vector>

class subtitle
{
public:
	int start = 0, end = 0;
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
	ComponentText* text;
};

ALIEN_FACTORY Subtitle* CreateSubtitle() {
	Subtitle* alien = new Subtitle();
	// To show in inspector here
	return alien;
} 
