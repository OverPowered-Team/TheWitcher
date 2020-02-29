#include "ComponentImage.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ReturnZ.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "mmgr/mmgr.h"

ComponentImage::ComponentImage(GameObject* obj) : ComponentUI(obj)
{
	ui_type = ComponentType::UI_IMAGE;
}

bool ComponentImage::DrawInspector()
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

	if (ImGui::CollapsingHeader("Image", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Image");

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

		ImGui::SameLine(85);
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
							ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
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
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				ClearTexture();
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Color");
		ImGui::SameLine(85);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		static bool set_Z = true;
		static Color col;
		col = current_color;
		if (ImGui::ColorEdit4("##RendererColor", &col, ImGuiColorEditFlags_Float)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			current_color = col;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}
		ImGui::Spacing();


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Image");
	}

	return true;
}

void ComponentImage::Reset()
{
	current_color = Color::White();
	ClearTexture();
}

void ComponentImage::SetComponent(Component* component)
{
	enabled = component->enabled;
	ComponentUI* ui = (ComponentUI*)component;
	current_color = ui->current_color;
	if (ui->texture != nullptr) {
		SetTexture(ui->texture);
	}
	else {
		ClearTexture();
	}
}

void ComponentImage::Clone(Component* clone)
{
	clone->enabled = enabled;
	ComponentUI* ui = (ComponentUI*)clone;
	ui->current_color = current_color;
	if (texture != nullptr) {
		ui->SetTexture(texture);
	}
	else {
		ui->ClearTexture();
	}
	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				ui->SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			ui->SetCanvas(nullptr);
		}
	}
}

void ComponentImage::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()) : "0");
	to_save->SetColor("Color", current_color);
}

void ComponentImage::LoadComponent(JSONArraypack* to_load)
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