#pragma once

#include "imgui/imgui.h"
#include "SDL/include/SDL.h"
#include <string>
#include <vector>
#include "Application.h"

struct ShortCut;

class Panel {

public:

	Panel(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~Panel();

	virtual void PanelLogic() {};

	const std::string& GetName();
	void ChangeEnable();
	bool IsEnabled();
	void SetEnable(const bool& enabled);
	const std::string GetPanelName() const;

	virtual void OnPlay() {}
	virtual void OnPause() {}
	virtual void OnStop() {}
	virtual void OnObjectSelect() {}
	virtual void OnAssetSelect() {}
	virtual void OnObjectDelete() {}
	virtual void OnAssetDelete() {}

private:

	virtual void OnPanelDesactive() {};

public:

	ShortCut* shortcut = nullptr;

protected:
	std::string panel_name;
	bool enabled = false;
};
