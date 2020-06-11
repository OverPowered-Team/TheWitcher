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

	}
	else
		RightClickMenu("Material");

	return true;
}

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
	
	ComponentMaterial* tmp = (ComponentMaterial*)component;
	this->SetMaterial(tmp->material);

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
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentMaterial* mat = (ComponentMaterial*)clone;
	mat->SetMaterial(material);
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


