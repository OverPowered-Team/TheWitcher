#pragma once

#include "Panel.h"
#include "ImGuizmos/ImGuizmo.h"

class ComponentCurve;

class PanelScene : public Panel {

public:

	PanelScene(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelScene();

	void PanelLogic();

private:

	void GuizmosControls();
	void ChangeWorldMode();
	void ChangeLocalMode();
	void Translate();
	void Rotate();
	void Scale();
	void GuizmosLogic();

	void GizmoCurve();

public:
	bool is_window_being_rendered = true;
	ImGuizmo::OPERATION guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmo_mode = ImGuizmo::MODE::WORLD;

public:
	float width = 960;
	float height = 540;

	float posX = 0;
	float posY = 0;

	ComponentCurve* curve = nullptr;
	int curve_index = 0;
	bool gizmo_curve = false;

	ImVec2 viewport_size = { 0.f, 0.f };
	ImVec2 viewport_min = { 0.f, 0.f };
	ImVec2 viewport_max = { 0.f, 0.f };
	ImVec2 current_viewport_size = { 0.f, 0.f };

};

