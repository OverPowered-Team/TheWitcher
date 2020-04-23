#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API FadeImages : public Alien {

	public:

		FadeImages();
		virtual ~FadeImages();
	
		void Start();
		void Update();
	private:
		float start_time_scene = 0;
		float current_time = 0;
		ComponentImage* image = nullptr;
		bool activated_event = false;
	public:
		float start_next_image = 0;
		float transition_duration;
		GameObject* second_image = nullptr;
};

ALIEN_FACTORY FadeImages* CreateFadeImages() {
	FadeImages* alien = new FadeImages();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->second_image);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->start_next_image);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->transition_duration);

	return alien;
} 
