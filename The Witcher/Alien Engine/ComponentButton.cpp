#include "ComponentButton.h"
#include "ComponentImage.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ComponentCanvas.h"
#include "ReturnZ.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "mmgr/mmgr.h"

ComponentButton::ComponentButton(GameObject* obj):ComponentUI(obj)
{
	ui_type = ComponentType::UI_BUTTON;

}

void ComponentButton::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()) : "0");
	to_save->SetColor("Color", current_color);
}

void ComponentButton::LoadComponent(JSONArraypack* to_load)
{
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };
	//UpdateVertex();

	enabled = to_load->GetBoolean("Enabled");
	current_color = to_load->GetColor("Color");
	u64 textureID = std::stoull(to_load->GetString("TextureID"));
	if (textureID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
		if (tex != nullptr) {
			SetTexture(tex);
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

		/*ImGui::PushID(this);
		ImGui::Text("Size:		"); ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("W", &size.x, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();
		ImGui::SameLine(); ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("H", &size.y, 0.5F, 0, 0, "%.3f", 1, game_object_attached->is_static))
			UpdateVertex();

		ImGui::PopID();*/

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Texture");

		ImGui::SameLine(120);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((texture == nullptr) ? "NULL" : std::string(texture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && texture != nullptr) {
			App->ui->panel_project->SelectFile(texture->GetAssetsPath(), App->resources->assets);
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
							SetTexture(texture);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (texture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && texture != nullptr) {
				ClearTexture();
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Idle Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorIdle", &idle_color, ImGuiColorEditFlags_Float)) {
			current_color = idle_color;
		}

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Hover Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorHover", &hover_color, ImGuiColorEditFlags_Float)) {

		}
		
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Click Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorClick", &clicked_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Pressed Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorPressed", &pressed_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Disabled Color");
		ImGui::SameLine(120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::ColorEdit4("##RendererColorDisabled", &disabled_color, ImGuiColorEditFlags_Float)) {

		}
		ImGui::Spacing();



		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Button");
	}

	return true;
}

bool ComponentButton::OnHover()
{
	if (active) {
		current_color = hover_color;
		CallListeners(&listenersOnHover);
	}
	return true;
}

bool ComponentButton::OnClick()
{
	if (active) {
		current_color = clicked_color;
		CallListeners(&listenersOnClick);
	}
	return true;
}

bool ComponentButton::OnPressed()
{
	if (active) {
		current_color = pressed_color;
		CallListeners(&listenersOnClickRepeat);
	}
	return true;
}

bool ComponentButton::OnRelease()
{
	if (active) {
		current_color = idle_color;
		CallListeners(&listenersOnRelease);
	}
	return true;
}

void ComponentButton::CallListeners(std::vector<std::function<void()>>* listeners)
{
	if (listeners != nullptr) {
		auto item = listeners->begin();
		for (; item != listeners->end(); ++item) {
			if (*item != nullptr) {
				try {
					(*item)();
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

void ComponentButton::SetActive(bool active)
{
	this->active = active;
	if (active) {
		current_color = idle_color;
	}
	else {
		current_color = disabled_color;
	}
}

void ComponentButton::AddListenerOnHover(std::function<void()> funct)
{
	listenersOnHover.push_back(funct);
}

void ComponentButton::AddListenerOnClick(std::function<void()> funct)
{
	listenersOnClick.push_back(funct);
}

void ComponentButton::AddListenerOnClickRepeat(std::function<void()> funct)
{
	listenersOnClickRepeat.push_back(funct);
}

void ComponentButton::AddListenerOnRelease(std::function<void()> funct)
{
	listenersOnRelease.push_back(funct);
}

//void ComponentButton::RemoveListenerOnHover(std::function<void()> funct)
//{
//	auto item = listenersOnHover.begin();
//	for (; item != listenersOnHover.end(); ++item) {
//		if (funct.target() == (*item).target()) {
//			listenersOnHover.erase(item);
//			break;
//		}
//	}
//}
//
//void ComponentButton::RemoveListenerOnClick(std::function<void()> funct)
//{
//	auto item = listenersOnClick.begin();
//	for (; item != listenersOnClick.end(); ++item) {
//		if (App->StringCmp(funct.target_type().name, (*item).target_type().name)) {
//			listenersOnClick.erase(item);
//			break;
//		}
//	}
//}
//
//void ComponentButton::RemoveListenerOnClickRepeat(std::function<void()> funct)
//{
//	auto item = listenersOnClickRepeat.begin();
//	for (; item != listenersOnClickRepeat.end(); ++item) {
//		if (App->StringCmp(funct.target_type().name, (*item).target_type().name)) {
//			listenersOnClickRepeat.erase(item);
//			break;
//		}
//	}
//}
//
//void ComponentButton::RemoveListenerOnRelease(std::function<void()> funct)
//{
//	auto item = listenersOnRelease.begin();
//	for (; item != listenersOnRelease.end(); ++item) {
//		if (App->StringCmp(funct.target_type().name, (*item).target_type().name)) {
//			listenersOnRelease.erase(item);
//			break;
//		}
//	}
//}
