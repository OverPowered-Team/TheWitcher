#include "PanelPhysics.h"
#include "ShortCutManager.h"
#include "ModulePhysics.h"
#include "Optick/include/optick.h"

PanelPhysics::PanelPhysics(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Physics", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelPhysics::~PanelPhysics()
{
}

void PanelPhysics::PanelLogic()
{
	OPTICK_EVENT();
	ImGui::Begin(panel_name.data(), &enabled);
	ImGui::Title("Debug All"); ImGui::Checkbox("##debug_all_physics", &App->physics->debug_physics);
	ImGui::End();
}
