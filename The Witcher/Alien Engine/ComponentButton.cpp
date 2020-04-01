#include "ComponentButton.h"
#include "ComponentImage.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ComponentCanvas.h"
#include "ReturnZ.h"
#include "ComponentScript.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "mmgr/mmgr.h"

ComponentButton::ComponentButton(GameObject* obj):ComponentUI(obj)
{
	ui_type = ComponentType::UI_BUTTON;
	tabbable = true;
}

void ComponentButton::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()).data() : "0");
	to_save->SetString("TextureIdleID", (idle_tex != nullptr) ? std::to_string(idle_tex->GetID()).data() : "0");
	to_save->SetString("TextureHoverID", (hover_tex != nullptr) ? std::to_string(hover_tex->GetID()).data() : "0");
	to_save->SetString("TextureClickedID", (clicked_tex != nullptr) ? std::to_string(clicked_tex->GetID()).data() : "0");
	to_save->SetString("TexturePressedID", (pressed_tex != nullptr) ? std::to_string(pressed_tex->GetID()).data() : "0");
	to_save->SetString("TextureDisabledID", (disabled_tex != nullptr) ? std::to_string(disabled_tex->GetID()).data() : "0");
	to_save->SetColor("ColorCurrent", current_color);
	to_save->SetColor("ColorIdle", idle_color);
	to_save->SetColor("ColorHover", hover_color);
	to_save->SetColor("ColorClicked", clicked_color);
	to_save->SetColor("ColorPressed", pressed_color);
	to_save->SetColor("ColorDisabled", disabled_color);

	//---------------------------------------------------------
	to_save->SetBoolean("HasListenersOnClick", !listenersOnClick.empty());
	if (!listenersOnClick.empty()) {
		JSONArraypack* onClickArray = to_save->InitNewArray("ListenersOnClick");
		auto item = listenersOnClick.begin();
		for (; item != listenersOnClick.end(); ++item) {
			onClickArray->SetAnotherNode();
			onClickArray->SetString(std::to_string(item - listenersOnClick.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnHover", !listenersOnHover.empty());
	if (!listenersOnHover.empty()) {
		JSONArraypack* onHoverArray = to_save->InitNewArray("ListenersOnHover");
		auto item = listenersOnHover.begin();
		for (; item != listenersOnHover.end(); ++item) {
			onHoverArray->SetAnotherNode();
			onHoverArray->SetString(std::to_string(item - listenersOnHover.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnClickRepeat", !listenersOnClickRepeat.empty());
	if (!listenersOnClickRepeat.empty()) {
		JSONArraypack* onClickRepeatArray = to_save->InitNewArray("ListenersOnClickRepeat");
		auto item = listenersOnClickRepeat.begin();
		for (; item != listenersOnClickRepeat.end(); ++item) {
			onClickRepeatArray->SetAnotherNode();
			onClickRepeatArray->SetString(std::to_string(item - listenersOnClickRepeat.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnRelease", !listenersOnRelease.empty());
	if (!listenersOnRelease.empty()) {
		JSONArraypack* onReleaseArray = to_save->InitNewArray("ListenersOnRelease");
		auto item = listenersOnRelease.begin();
		for (; item != listenersOnRelease.end(); ++item) {
			onReleaseArray->SetAnotherNode();
			onReleaseArray->SetString(std::to_string(item - listenersOnRelease.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnExit", !listenersOnExit.empty());
	if (!listenersOnExit.empty()) {
		JSONArraypack* onExitArray = to_save->InitNewArray("ListenersOnExit");
		auto item = listenersOnExit.begin();
		for (; item != listenersOnExit.end(); ++item) {
			onExitArray->SetAnotherNode();
			onExitArray->SetString(std::to_string(item - listenersOnExit.begin()).data(), (*item).first.data());
		}
	}
	//---------------------------------------------------------------
	to_save->SetString("SelectOnUp", std::to_string(select_on_up).data());
	to_save->SetString("SelectOnDown", std::to_string(select_on_down).data());
	to_save->SetString("SelectOnRight", std::to_string(select_on_right).data());
	to_save->SetString("SelectOnLeft", std::to_string(select_on_left).data());
}

void ComponentButton::LoadComponent(JSONArraypack* to_load)
{
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");
	
	current_color = to_load->GetColor("ColorCurrent");
	idle_color = to_load->GetColor("ColorIdle");
	hover_color = to_load->GetColor("ColorHover");
	clicked_color = to_load->GetColor("ColorClicked");
	pressed_color = to_load->GetColor("ColorPressed");
	disabled_color = to_load->GetColor("ColorDisabled");

	select_on_up = std::stoull(to_load->GetString("SelectOnUp"));
	select_on_down = std::stoull(to_load->GetString("SelectOnDown"));
	select_on_right = std::stoull(to_load->GetString("SelectOnRight"));
	select_on_left = std::stoull(to_load->GetString("SelectOnLeft"));

	//-------------------------------------------------------------
	if (to_load->GetBoolean("HasListenersOnClick")) {
		JSONArraypack* onClickListeners = to_load->GetArray("ListenersOnClick");
		for (int i = 0; i < onClickListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onClickListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnClick.push_back(pair);
			onClickListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnHover")) {
		JSONArraypack* onHoverListeners = to_load->GetArray("ListenersOnHover");
		for (int i = 0; i < onHoverListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onHoverListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnHover.push_back(pair);
			onHoverListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnClickRepeat")) {
		JSONArraypack* onClickRepeatListeners = to_load->GetArray("ListenersOnClickRepeat");
		for (int i = 0; i < onClickRepeatListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onClickRepeatListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnClickRepeat.push_back(pair);
			onClickRepeatListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnRelease")) {
		JSONArraypack* onReleaseListeners = to_load->GetArray("ListenersOnRelease");
		for (int i = 0; i < onReleaseListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onReleaseListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnRelease.push_back(pair);
			onReleaseListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnExit")) {
		JSONArraypack* onExitListeners = to_load->GetArray("ListenersOnExit");
		for (int i = 0; i < onExitListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onExitListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnExit.push_back(pair);
			onExitListeners->GetAnotherNode();
		}
	}
	//-------------------------------------------------------------


	u64 textureID = std::stoull(to_load->GetString("TextureID"));
	if (textureID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
		if (tex != nullptr) {
			SetTexture(tex);
		}
	}
	u64 textureIdleID = std::stoull(to_load->GetString("TextureIdleID"));
	if (textureIdleID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureIdleID);
		if (tex != nullptr) {
			SetStateTexture(UIState::Idle, tex);
		}
	}
	u64 textureHoverID = std::stoull(to_load->GetString("TextureHoverID"));
	if (textureHoverID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureHoverID);
		if (tex != nullptr) {
			SetStateTexture(UIState::Hover, tex);
		}
	}
	u64 textureClickedID = std::stoull(to_load->GetString("TextureClickedID"));
	if (textureClickedID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureClickedID);
		if (tex != nullptr) {
			SetStateTexture(UIState::Click, tex);
		}
	}
	u64 texturePressedID = std::stoull(to_load->GetString("TexturePressedID"));
	if (texturePressedID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(texturePressedID);
		if (tex != nullptr) {
			SetStateTexture(UIState::Pressed, tex);
		}
	}
	u64 textureDisabledID = std::stoull(to_load->GetString("TextureDisabledID"));
	if (textureDisabledID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureDisabledID);
		if (tex != nullptr) {
			SetStateTexture(UIState::Disabled, tex);
		}
	}


	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			SetCanvas(nullptr);
		}
	}

	App->objects->first_assigned_selected = false;
}

void ComponentButton::HandleAlienEvent(const AlienEvent& e)
{
	
	switch (e.type)
	{
	case AlienEventType::SCRIPT_DELETED: {
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached)
		{
			//Delete on Click
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnClick.begin(); item != listenersOnClick.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnClick.erase(item);
						//delete this from listeners on Click
						break;
					}
				}
		
			}

			//Delete on Hover
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnHover.begin(); item != listenersOnHover.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnHover.erase(item);
						//delete this from listeners on Click
			
						break;
					}
				}
			}

			//Delete on pressed
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnClickRepeat.begin(); item != listenersOnClickRepeat.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnClickRepeat.erase(item);
						//delete this from listeners on Click
						break;
					}
				}
			
			}

			//delete on release
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnRelease.begin(); item != listenersOnRelease.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnRelease.erase(item);
						break;
					}
				}
			}

			//delete on exit
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnExit.begin(); item != listenersOnExit.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnExit.erase(item);
						break;
					}
				}
			}
			
		}
		break; }

	default: {
		break; }
	}
}

bool ComponentButton::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Button", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Button");

		ImGui::Spacing();
		if (ImGui::TreeNode("Textures"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Idle Texture");

			ImGui::SameLine(150);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button((idle_tex == nullptr) ? "NULL" : std::string(idle_tex->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && idle_tex != nullptr) {
				App->ui->panel_project->SelectFile(idle_tex->GetAssetsPath(), App->resources->assets);
			}

			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (texture != nullptr) {
								SetStateTexture(UIState::Idle, texture);
								SetTexture(texture);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (idle_tex != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("X##idletex") && idle_tex != nullptr) {
					ClearStateTexture(UIState::Idle);
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Hover Texture");

			ImGui::SameLine(150);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button((hover_tex == nullptr) ? "NULL" : std::string(hover_tex->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && hover_tex != nullptr) {
				App->ui->panel_project->SelectFile(hover_tex->GetAssetsPath(), App->resources->assets);
			}

			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (texture != nullptr) {
								SetStateTexture(UIState::Hover, texture);

							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (hover_tex != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("X##hovertex") && hover_tex != nullptr) {
					ClearStateTexture(UIState::Hover);
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Clicked Texture");

			ImGui::SameLine(150);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button((clicked_tex == nullptr) ? "NULL" : std::string(clicked_tex->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && clicked_tex != nullptr) {
				App->ui->panel_project->SelectFile(clicked_tex->GetAssetsPath(), App->resources->assets);
			}

			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (texture != nullptr) {
								SetStateTexture(UIState::Click, texture);

							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (clicked_tex != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("X##clickedtex") && clicked_tex != nullptr) {
					ClearStateTexture(UIState::Click);
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Pressed Texture");

			ImGui::SameLine(150);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button((pressed_tex == nullptr) ? "NULL" : std::string(pressed_tex->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && pressed_tex != nullptr) {
				App->ui->panel_project->SelectFile(pressed_tex->GetAssetsPath(), App->resources->assets);
			}

			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (texture != nullptr) {
								SetStateTexture(UIState::Pressed, texture);

							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (pressed_tex != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("X##pressedtex") && pressed_tex != nullptr) {
					ClearStateTexture(UIState::Pressed);
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Disabled Texture");

			ImGui::SameLine(150);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button((disabled_tex == nullptr) ? "NULL" : std::string(disabled_tex->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && disabled_tex != nullptr) {
				App->ui->panel_project->SelectFile(disabled_tex->GetAssetsPath(), App->resources->assets);
			}

			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (texture != nullptr) {
								SetStateTexture(UIState::Disabled, texture);

							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (disabled_tex != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("X##disabledtex") && disabled_tex != nullptr) {
					ClearStateTexture(UIState::Disabled);
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::TreePop();

		}
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		//------------------------SCRIPTS----------------------------
		if (ImGui::TreeNode("Script Listeners")) {
			//--------------
			if (ImGui::TreeNode("Functions Added")) {
				if (ImGui::TreeNode("On Click Added")) {
					for (auto item = listenersOnClick.begin(); item != listenersOnClick.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnClick.begin(), item) + 3876987);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							listenersOnClick.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
							//delete function
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Hover Added")) {
					for (auto item = listenersOnHover.begin(); item != listenersOnHover.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnHover.begin(), item) + 58613446);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							listenersOnHover.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
							//delete function
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);

					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Pressed Added")) {
					for (auto item = listenersOnClickRepeat.begin(); item != listenersOnClickRepeat.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnClickRepeat.begin(), item) + 4987543);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnClickRepeat.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Release Added")) {
					for (auto item = listenersOnRelease.begin(); item != listenersOnRelease.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnRelease.begin(), item) + 7654367);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnRelease.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Exit Added")) {
					for (auto item = listenersOnExit.begin(); item != listenersOnExit.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnExit.begin(), item) + 3812667);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnExit.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			//--------------

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			std::vector<ComponentScript*> scripts = game_object_attached->GetComponents<ComponentScript>();
			if (ImGui::TreeNode("Functions To Add")) {
				if (!scripts.empty()) {
					if (ImGui::TreeNode("On Click To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnClick((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Hover To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnHover((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Pressed To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnClickRepeat((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Release To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnRelease((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Exit To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnExit((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
				}

				else {
					ImGui::Text("No Scripts attached");
				}
				ImGui::TreePop();
			}


			ImGui::TreePop();
		}
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		//------------------------COLOR-------------------------------
		if (ImGui::TreeNode("Colors"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Idle Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorIdle", &idle_color, ImGuiColorEditFlags_Float)) {
				current_color = idle_color;
			}

			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Hover Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorHover", &hover_color, ImGuiColorEditFlags_Float)) {

			}

			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Click Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorClick", &clicked_color, ImGuiColorEditFlags_Float)) {

			}
			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Pressed Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorPressed", &pressed_color, ImGuiColorEditFlags_Float)) {

			}
			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Disabled Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorDisabled", &disabled_color, ImGuiColorEditFlags_Float)) {

			}

			ImGui::TreePop();
		}

		//---------------------END COLOR-----------------------------
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::TreeNode("Navigation"))
		{
			//--------------------UP-----------------------------
			ImGui::Text("Select on Up");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_up) != nullptr)
				ImGui::Button((select_on_up != -1) ? std::string((App->objects->GetGameObjectByID(select_on_up)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject * *)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_up = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_up);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectUpBut")) {
				if (select_on_up != -1) {
					select_on_up = -1;
				}
			}
			ImGui::PopStyleColor(3);
			ImGui::Spacing();

			//--------------------DOWN-----------------------------
			ImGui::Text("Select on Down");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_down) != nullptr)
				ImGui::Button((select_on_down != -1) ? std::string((App->objects->GetGameObjectByID(select_on_down)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject * *)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_down = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_down);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectDownBut")) {
				if (select_on_down != -1) {
					select_on_down = -1;
				}
			}
			ImGui::PopStyleColor(3);




			ImGui::Spacing();

			//--------------------RIGHT-----------------------------
			ImGui::Text("Select on Right");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_right) != nullptr)
				ImGui::Button((select_on_right != -1) ? std::string((App->objects->GetGameObjectByID(select_on_right)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_right = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_right);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectRightBut")) {
				if (select_on_right != -1) {
					select_on_right = -1;
				}
			}
			ImGui::PopStyleColor(3);





			ImGui::Spacing();

			//--------------------LEFT-----------------------------
			ImGui::Text("Select on Left");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_left) != nullptr)
				ImGui::Button((select_on_left != -1) ? std::string((App->objects->GetGameObjectByID(select_on_left)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_left = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_left);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectLeftBut")) {
				if (select_on_left != -1) {
					select_on_left = -1;
				}
			}
			ImGui::PopStyleColor(3);

			ImGui::Spacing();
			//----------------------------------------------------------------------

			ImGui::TreePop();
		}

		ImGui::Spacing();


		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
	else {
		RightClickMenu("Button");
	}

	return true;
}

bool ComponentButton::OnIdle()
{
	if (active) {
		current_color = idle_color;
		if (idle_tex != nullptr && texture != idle_tex) {
			SetTexture(idle_tex);
			SetSize(idle_tex->width, idle_tex->height);
		}
	}
	return true;
}

bool ComponentButton::OnHover()
{
	if (active) {
		current_color = hover_color;
		if (hover_tex != nullptr){
			SetTexture(hover_tex);
			SetSize(hover_tex->width, hover_tex->height);
		}
		CallListeners(&listenersOnHover);
	}
	return true;
}

bool ComponentButton::OnClick()
{
	if (active) {
		current_color = clicked_color;
		if (clicked_tex != nullptr) {
			SetTexture(clicked_tex);
			SetSize(clicked_tex->width, clicked_tex->height);
		}
		CallListeners(&listenersOnClick);
	}
	return true;
}

bool ComponentButton::OnPressed()
{
	if (active) {
		current_color = pressed_color;
		if (pressed_tex != nullptr) {
			SetTexture(pressed_tex);
			SetSize(pressed_tex->width, pressed_tex->height);
		}
		CallListeners(&listenersOnClickRepeat);
	}
	return true;
}

bool ComponentButton::OnRelease()
{
	if (active) {
		current_color = hover_color;
		if (hover_tex != nullptr) {
			SetTexture(hover_tex);
			SetSize(hover_tex->width, hover_tex->height);
		}
		CallListeners(&listenersOnRelease);
	}
	return true;
}

bool ComponentButton::OnExit()
{
	if (active) 
	{
		//baina loka
		CallListeners(&listenersOnExit);
	}
	return true;
}

void ComponentButton::CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners)
{
	if (listeners != nullptr) {
		auto item = listeners->begin();
		for (; item != listeners->end(); ++item) {
			if ((*item).second != nullptr) {
				try {
					(*item).second();
				}
				catch (...) {
				#ifndef GAME_VERSION
					LOG_ENGINE("Error when calling a listener function of a button");
					App->ui->SetError();
				#endif
				}
			}
		}
	}
}

void ComponentButton::SetStateTexture(UIState state, ResourceTexture* tex)
{
	switch (state)
	{
	case Idle: {
		if (tex != nullptr && tex != idle_tex) {
			tex->IncreaseReferences();
			if (idle_tex != nullptr) {
				idle_tex->DecreaseReferences();
			}
			idle_tex = tex;
		}
		break; }
	case Hover: {
		if (tex != nullptr && tex != hover_tex) {
			tex->IncreaseReferences();
			if (hover_tex != nullptr) {
				hover_tex->DecreaseReferences();
			}
			hover_tex = tex;
		}
		break; }
	case Click: {
		if (tex != nullptr && tex != clicked_tex) {
			tex->IncreaseReferences();
			if (clicked_tex != nullptr) {
				clicked_tex->DecreaseReferences();
			}
			clicked_tex = tex;
			
		}
		break; }
	case Pressed: {
		if (tex != nullptr && tex != pressed_tex) {
			tex->IncreaseReferences();
			if (pressed_tex != nullptr) {
				pressed_tex->DecreaseReferences();
			}
			pressed_tex = tex;
			
		}
		break; }
	case Disabled: {
		if (tex != nullptr && tex != disabled_tex) {
			tex->IncreaseReferences();
			if (disabled_tex != nullptr) {
				disabled_tex->DecreaseReferences();
			}
			disabled_tex = tex;
			
		}
		break; }
	default: {
		break; }
	}
	
	
}

void ComponentButton::ClearStateTexture(UIState state)
{
	switch (state)
	{
	case Idle: {
		if (idle_tex != nullptr) {
			idle_tex->DecreaseReferences();
			idle_tex = nullptr;
		}
		break; }
	case Hover: {
		if (hover_tex != nullptr) {
			hover_tex->DecreaseReferences();
			hover_tex = nullptr;
		}
		break; }
	case Click: {
		if (clicked_tex != nullptr) {
			clicked_tex->DecreaseReferences();
			clicked_tex = nullptr;
		}
		break; }
	case Pressed: {
		if (pressed_tex != nullptr) {
			pressed_tex->DecreaseReferences();
			pressed_tex = nullptr;
		}
		break; }
	case Disabled: {
		if (disabled_tex != nullptr) {
			disabled_tex->DecreaseReferences();
			disabled_tex = nullptr;
		}
		break; }
	default: {
		break; }
	}
}


void ComponentButton::SetActive(bool active)
{
	this->active = active;
	if (active) {
		current_color = idle_color;

		if (idle_tex != nullptr) {
			SetTexture(idle_tex);
			SetSize(idle_tex->width, idle_tex->height);
		}
	}
	else {
		current_color = disabled_color;
		if (disabled_tex != nullptr) {
			SetTexture(disabled_tex);
			SetSize(disabled_tex->width, disabled_tex->height);
		}
	}
}

void ComponentButton::AddListenerOnHover(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnHover, name))
	{
		listenersOnHover.push_back(pair);
	}
}

void ComponentButton::AddListenerOnClick(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClick, name))
	{
		listenersOnClick.push_back(pair);
	}
}

void ComponentButton::AddListenerOnClickRepeat(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClickRepeat, name))
	{
		listenersOnClickRepeat.push_back(pair);
	}
}

void ComponentButton::AddListenerOnRelease(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnRelease, name))
	{
		listenersOnRelease.push_back(pair);
	}
}

void ComponentButton::AddListenerOnExit(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnExit, name))
	{
		listenersOnExit.push_back(pair);
	}
}

bool ComponentButton::CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name)
{
	if (listeners != nullptr) {

		for (auto item = listeners->begin(); item != listeners->end(); ++item){
		
			if ((*item).first == name)
				return true;
		}
	}
	return false;
}

