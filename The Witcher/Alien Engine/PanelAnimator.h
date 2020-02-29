#pragma once

class ImVec2;

#include "Panel.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "NodeEditor/Include/imgui_node_editor.h"
#include "ComponentAnimator.h"
#include "ResourceAnimatorController.h"

class PanelAnimator :
	public Panel
{
public:
	PanelAnimator(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelAnimator();

	bool FillInfo();
	void PanelLogic();

	void DrawStates();
	void DrawTransitions();
	void HandleContextMenu();
	void CreateState();
	void HandleDropLink();
	void ShowStatePopup();
	void ShowNewStatePopup();
	void ShowLinkPopup();
	void Start();
	bool IsInside(const float2& pos) const;

	void DrawParameterList();
	void OnAssetSelect();
	void OnAssetDelete();
	void OnObjectSelect();
	void OnObjectDelete();

public:
	ResourceAnimatorController* current_animator = nullptr;

	void SetCurrentResourceAnimatorController(ResourceAnimatorController* animator);

private:
	bool stats = false;
	ImVec2 screen_pos;
	float w, h;
	bool clip_select = false;
	int pin_in_id = 100;
	int pin_out_id = 200;
	int link_id = 300;
	int unique_id = 1;
	ImVec2 new_node_pos;

	std::string context_node;
	ax::NodeEditor::NodeId context_node_id;
	uint selected_link_index = 0;

	//creation management
	ax::NodeEditor::PinId new_node_id = 0;
	State* source_state = nullptr;

	bool show_transition_window = false;
};

