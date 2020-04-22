#pragma once

#include "Panel.h"

struct UniformData
{
	char uniform_name[MAX_PATH];
	Color create_color{ 1,1,1 };
	int type = 0;
};

class PanelCreateMaterial : public Panel {

public:

	PanelCreateMaterial(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelCreateMaterial();
	void PanelLogic();
	void SetUniforms();

	void OnPanelDesactive();

private:
	char path[MAX_PATH] = "path";
	std::vector<UniformData*>uniforms;
};

