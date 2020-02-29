#pragma once

#include "Panel.h"
#include "ResourceAnimation.h"

class ComponentAnimator;

class PanelAnimTimeline : public Panel
{
public:

	PanelAnimTimeline(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelAnimTimeline();

	
	void PanelLogic();

public:

	bool changed = true;
	

private:

	bool FillInfo();
	void Play();
	void Stop();
	void CleanUp();
	void MoveBones(GameObject* go);
	ComponentAnimator* component_animator = nullptr;
	ResourceAnimation::Channel* channel = nullptr;
	ResourceAnimatorController* animator = nullptr;
	ResourceAnimation* current_animation = nullptr;
	std::vector<ResourceAnimation*> animations;
	float num_frames;

	float button_position = 0.0f;

	bool setted = false;
	bool scrolled = false;

	float recSize = 1000;
	float zoom = 25;
	float speed = 0.5f;
	float progress = 0.0f;

	float windows_size = 1000.0f;

	bool play = false;
	bool pause = false;
	bool stop = false;
	bool in_game = false;

	bool dragging = false;

	float animation_time = 0.0f;
	float aux_time = 0.0f;
	int margin = 200;

	int key = 0;


	//Events
	void ShowNewEventPopUp();

	void CreateAnimationEvent();

};
