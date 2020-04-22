#include "Application.h"
#include "PanelCreateMaterial.h"
#include "ResourceShader.h"
#include "ModuleObjects.h"
#include "Shapes.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ReturnZ.h"
#include "ComponentTransform.h"

PanelCreateMaterial::PanelCreateMaterial(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("CreateMaterial", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
}

PanelCreateMaterial::~PanelCreateMaterial()
{
}

void PanelCreateMaterial::PanelLogic()
{
	static float x = 0.0f;
	static float y = 0.f;
	static float z = 0.f;

	static int objects_combo = 0;

	ImGui::OpenPopup(panel_name.c_str());
	ImGui::SetNextWindowContentWidth(300);
	if (ImGui::BeginPopupModal(panel_name.c_str(), &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::IsWindowHovered())
			App->camera->is_scene_hovered = false;
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::SetCursorPosX(10);
		ImGui::InputText("Save Path", path, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Position");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Columns(3, 0, false);
		ImGui::InputFloat("X", &x, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::InputFloat("Y", &y, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::InputFloat("Z", &z, 0, 0, 2);
		ImGui::NextColumn();
		ImGui::Columns(1);

		ImGui::Spacing();

		for (int i = 0; i < uniforms.size(); ++i)
		{
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text(uniforms[i]->uniform_name);
			if(uniforms[i]->type == 0)
				ImGui::Text("no type");
			if (uniforms[i]->type == 1)
				ImGui::Text("Vec4");
		}
		ImGui::Spacing();
		ImGui::Spacing();

		SetUniforms();

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Create", { ImGui::GetWindowWidth() - 16,25 }))
		{
			ResourceShader* n_shader = new ResourceShader((SHADERS_FOLDER + std::string(path) + std::string(".shader")).c_str());
			n_shader->IncreaseReferences();
			n_shader->CreateShaderDoc(1,path);
			n_shader->CreateMetaData(n_shader->GetID());
			App->resources->AddResource(n_shader);
			ChangeEnable();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::EndPopup();
	}
}

void PanelCreateMaterial::SetUniforms()
{
	static char uniform_name[MAX_PATH] = "name";
	static Color create_color;
	ImGui::Separator();
	ImGui::Text("Variable name: "); ImGui::SameLine();
	ImGui::InputText("###variable_name", uniform_name, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
	ImGui::Text("Type: "); ImGui::SameLine();

	static int uniform_type_int = 0;

	ImGui::Combo(" ", &uniform_type_int, "No type\0Vec4");

	ImGui::ColorEdit3("Select Color", (float*)&create_color, ImGuiColorEditFlags_Float);

	if (ImGui::Button("Create Uniform"))
	{
		UniformData* new_u = new UniformData();
		new_u->create_color = create_color;
		strncpy(new_u->uniform_name, uniform_name, MAX_PATH);
		new_u->type = uniform_type_int;
		uniforms.push_back(new_u);
	}
}

void PanelCreateMaterial::OnPanelDesactive()
{

}
