#include "PanelGame.h"
#include "Color.h"
#include "imgui/imgui.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"
#include "FileNode.h"
#include "ShortCutManager.h"
#include "PanelSceneSelector.h"
#include "ModuleObjects.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"


PanelGame::PanelGame(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
#ifndef GAME_VERSION
	shortcut = App->shortcut_manager->AddShortCut("Panel Game", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
#endif
	game_focused = true;
}

PanelGame::~PanelGame()
{
}

void PanelGame::PanelLogic()
{
	OPTICK_EVENT();
	ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (App->objects->game_cameras.empty())
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() * 0.5f) - 80);
		ImGui::SetCursorPosY((ImGui::GetWindowHeight() * 0.5f));
		ImGui::Text("No Cameras Available :(");
		App->objects->game_viewport->active = false;
	}
	else {
		viewport_min = ImGui::GetCursorScreenPos();
		viewport_max = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail();

		current_viewport_size = ImGui::GetContentRegionAvail();
		posX = ImGui::GetWindowPos().x + ImGui::GetCursorPosX();
		posY = ImGui::GetWindowPos().y + ImGui::GetCursorPosY() - ImGui::GetCurrentWindow()->TitleBarHeight() - 10;
		
		ImGui::Image((ImTextureID)App->objects->game_viewport->GetTexture(), ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));
		width = current_viewport_size.x;
		height = current_viewport_size.y;

		App->objects->game_viewport->SetPos(float2(viewport_min.x, viewport_min.y));  

		if (!(current_viewport_size == viewport_size)) 
		{
			viewport_size = current_viewport_size;
			App->objects->game_viewport->SetSize(viewport_size.x, viewport_size.y);
		}

		App->objects->game_viewport->active = enabled; 
	}

	if (ImGui::IsWindowFocused()) {
		game_focused = true;
	}
	else {
		game_focused = false;
	}

	ImDrawList* list = ImGui::GetWindowDrawList();
	if (list != nullptr && list->CmdBuffer.size() > 1) {
		App->objects->game_viewport->active = true;
	}
	else {
		App->objects->game_viewport->active = false;
	}
	
	ImGui::End();
}
