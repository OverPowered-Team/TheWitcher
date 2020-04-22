#pragma once
#include "Panel.h"

class PanelPhysics : public Panel {

public:

	PanelPhysics(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelPhysics();

	void PanelLogic();
	void PopUpAddLayer();
	void PopUpRemoveLayer();

private:

	bool open_add_popup = false;
	bool open_remove_popup = false;
	int current_layer = 0;

};