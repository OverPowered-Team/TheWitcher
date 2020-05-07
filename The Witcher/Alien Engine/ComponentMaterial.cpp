#include <fstream>
#include "ComponentMaterial.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"
#include "ReturnZ.h"
#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(GameObject* attach) : Component(attach)
{
	type = ComponentType::MATERIAL;

	material = App->resources->default_material;
	material->IncreaseReferences();
}

ComponentMaterial::~ComponentMaterial()
{
	if (material != nullptr)
		material->DecreaseReferences();
}

bool ComponentMaterial::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
	}
	ImGui::PopID();
	ImGui::SameLine();

	//InspectorShaderProperties();

	if (ImGui::CollapsingHeader("Material", &not_destroy))
	{
		material->DisplayMaterialOnInspector();
		
		RightClickMenu("Material");

		ImGui::Checkbox("Cast shadow:", &game_object_attached->cast_shadow);
		//InspectorShaderProperties();

		/*ImGui::Spacing();
		static bool set_Z = true;
		ImGui::Spacing();
		static Color col;
		col = color;
		if (ImGui::ColorEdit3("Material Color", &col, ImGuiColorEditFlags_Float)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			color = col;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}

		if (ImGui::SliderFloat("Alpha", &col.a, 0.0F, 1.0F)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			color.a = col.a;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}*/
	}
	else
		RightClickMenu("Material");

	return true;
}

//void ComponentMaterial::InspectorShaderProperties()
//{
//	/* Shaders */
//	if (material->used_shader != nullptr)
//	{
//		/* Set shader unifroms from Inspector */
//		if (material->used_shader->ChangeTemplate())
//		{
//			if (material->used_shader != nullptr) {
//				material->used_shader->DecreaseReferences();
//			}
//
//			file_to_edit = material->used_shader->path;
//		}
//
//		material->used_shader->HierarchyUniforms();
//
//		ImGui::Separator();
//		ImGui::Text("Current shader: "); ImGui::SameLine();
//		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), material->used_shader->path.c_str());
//		ImGui::SameLine();
//		if (ImGui::Button("Edit shader"))
//		{
//			{
//				std::ifstream t(file_to_edit.c_str());
//				if (t.good())
//				{
//					std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
//					shader_editor.SetText(str);
//				}
//			}
//
//			show_shader_text_editor = true;
//		}
//		ImGui::SameLine();
//
//		static const char* text_compilation_shader = "";
//		static bool compiled_shader_success = false;
//		if (ImGui::Button("Compile shader")) // TODO: Compile automatically when we save and show error
//		{
//			if (material->used_shader->ParseAndCreateShader() == 0)
//			{
//				compiled_shader_success = false;
//				text_compilation_shader = "Shader compilation unsuccessful. Please fix your code.";
//				LOG_ENGINE("Shader compiled unsuccessfully...");
//			}
//			else
//			{
//				compiled_shader_success = true;
//				text_compilation_shader = "Shader compilation successful.";
//				LOG_ENGINE("Shader compiled successfully.");
//			}
//		}
//
//		compiled_shader_success ? ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), text_compilation_shader)
//			: ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), text_compilation_shader);
//
//		// Can select desired shader in the shaders folder
//
//		if (show_shader_text_editor)
//		{
//			ShowShaderTextEditor();
//		}
//	}
//}

void ComponentMaterial::Reset()
{
	if (material != nullptr)
		material->DecreaseReferences();
	
	material = App->resources->default_material;
	if(material != nullptr)
		material->IncreaseReferences();
}

void ComponentMaterial::SetComponent(Component* component)
{
	/*if (component->GetType() == type) {

		ComponentMaterial* material = (ComponentMaterial*)component;
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = material->texture;
		if (texture != nullptr) {
			texture->IncreaseReferences();
		}

		color = material->color;
	}*/

	// Newest
	/*if (component->GetType() == type) {

		ComponentMaterial* material = (ComponentMaterial*)component;
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = material->texture;
		if (texture != nullptr) {
			texture->IncreaseReferences();
		}
		if (used_shader != nullptr) {
			used_shader->DecreaseReferences();
		}
		used_shader = material->used_shader;
		if (used_shader != nullptr) {
			used_shader->IncreaseReferences();
		}

		color = material->color;
	}*/
}

void ComponentMaterial::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetBoolean("HasMaterial", (material != nullptr) ? true : false);
	if (material != nullptr) {
		to_save->SetString("MaterialID", std::to_string(material->GetID()).data());
	}
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentMaterial::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");

	if (to_load->GetBoolean("HasMaterial")) {
		u64 ID = std::stoull(to_load->GetString("MaterialID"));
		SetMaterial((ResourceMaterial*)App->resources->GetResourceWithID(ID));
	}
	ID = std::stoull(to_load->GetString("ID"));
}


void ComponentMaterial::Clone(Component* clone)
{
	/*clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentMaterial* mat = (ComponentMaterial*)clone;
	mat->color = color;
	mat->texture = texture;
	if (texture != nullptr) {
		++texture->references;
	}
	mat->texture_activated = texture_activated;*/


	// Newest
	/*clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentMaterial* mat = (ComponentMaterial*)clone;
	mat->color = color;
	mat->texture = texture;
	mat->used_shader = used_shader;
	if (texture != nullptr) {
		++texture->references;
	}
	if (used_shader != nullptr) {
		++used_shader->references;
	}
	mat->texture_activated = texture_activated;*/
}

void ComponentMaterial::SetTexture(ResourceTexture* tex, TextureType texType = TextureType::DIFFUSE)
{
	if (tex == nullptr)
	{
		RemoveTexture(texType);
		return;
	}

	if (tex == material->GetTexture(texType)) // Unity does not do this, but I think it should
		return;

	// Look for an already created material (with the same name as the texture) that has the same texture
	//ResourceMaterial* foundMaterial = App->resources->GetMaterialByName(tex->GetName());
	//if (foundMaterial != nullptr && foundMaterial->GetTexture(texType) == tex)
	//{
	//	SetMaterial(foundMaterial);
	//}
	//else // Create a new material
	//{
	//	//if(!(App->StringCmp(material->GetName(),tex->GetName()) && !material->HasTexture())) // TODO: Should discuss this
	//	SetMaterial(App->resources->CreateMaterial(tex->GetName()));
	//}

	if (material == App->resources->default_material)
	{
		SetMaterial(App->resources->CreateMaterial(tex->GetName()));
	}

	// Finally assign the texture to the desired material
	material->SetTexture(tex, texType);
}

void ComponentMaterial::RemoveTexture(TextureType texType = TextureType::DIFFUSE)
{
	material->RemoveTexture(texType);
}

const ResourceTexture* ComponentMaterial::GetTexture(TextureType texType = TextureType::DIFFUSE) const
{
	return material->GetTexture(texType);
}

void ComponentMaterial::SetMaterial(ResourceMaterial* mat)
{
	if (mat == nullptr || mat == material)	// If it is nullptr should we assign it to default material?
		return; 

	if (material != nullptr)
		material->DecreaseReferences();

	material = mat;
	material->IncreaseReferences(); 
}

const ResourceMaterial* ComponentMaterial::GetMaterial() const
{
	return material;
}

ResourceShader* ComponentMaterial::GetUsedShader() const
{
	return material->used_shader;
}

bool ComponentMaterial::IsTransparent() const
{
	return material->IsTransparent();
}



void ComponentMaterial::ShowShaderTextEditor()
{
	//auto cpos = shader_editor.GetCursorPosition();
	//ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	//ImGui::Begin("Shader Editor", &show_shader_text_editor, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	//ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Save"))
	//		{
	//			auto textToSave = shader_editor.GetText();

	//			// Save text assets folder
	//			App->file_system->Save(file_to_edit.c_str(), textToSave.c_str(), textToSave.size());

	//			// Save text library folder
	//			App->file_system->Save(material->used_shader->GetLibraryPath(), textToSave.c_str(), textToSave.size());
	//		}

	//		ImGui::EndMenu();
	//	}
	//	if (ImGui::BeginMenu("Edit"))
	//	{
	//		bool ro = shader_editor.IsReadOnly();
	//		if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
	//			shader_editor.SetReadOnly(ro);
	//		ImGui::Separator();

	//		if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && shader_editor.CanUndo()))
	//			shader_editor.Undo();
	//		if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && shader_editor.CanRedo()))
	//			shader_editor.Redo();

	//		ImGui::Separator();

	//		if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, shader_editor.HasSelection()))
	//			shader_editor.Copy();
	//		if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && shader_editor.HasSelection()))
	//			shader_editor.Cut();
	//		if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && shader_editor.HasSelection()))
	//			shader_editor.Delete();
	//		if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
	//			shader_editor.Paste();

	//		ImGui::Separator();

	//		if (ImGui::MenuItem("Select all", nullptr, nullptr))
	//			shader_editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(shader_editor.GetTotalLines(), 0));

	//		ImGui::EndMenu();
	//	}

	//	if (ImGui::BeginMenu("View"))
	//	{
	//		if (ImGui::MenuItem("Dark palette"))
	//			shader_editor.SetPalette(TextEditor::GetDarkPalette());
	//		if (ImGui::MenuItem("Light palette"))
	//			shader_editor.SetPalette(TextEditor::GetLightPalette());
	//		if (ImGui::MenuItem("Retro blue palette"))
	//			shader_editor.SetPalette(TextEditor::GetRetroBluePalette());
	//		ImGui::EndMenu();
	//	}

	//	ImGui::EndMenuBar();
	//}

	//ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, shader_editor.GetTotalLines(),
	//	shader_editor.IsOverwrite() ? "Ovr" : "Ins",
	//	shader_editor.CanUndo() ? "*" : " ",
	//	shader_editor.GetLanguageDefinition().mName.c_str(), file_to_edit.c_str());

	//shader_editor.Render("TextEditor");

	//ImGui::End();
}
