#include "ResourceMaterial.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ResourceShader.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"
#include "FileNode.h"
#include "JSONfilepack.h"

#include "Optick/include/optick.h"
#include "glew/include/glew.h"
#include "mmgr/mmgr.h"

ResourceMaterial::ResourceMaterial() : Resource()
{
	type = ResourceType::RESOURCE_MATERIAL;

	for (uint i = 0; i < (uint)TextureType::MAX; ++i)
	{
		textures[i].first = NO_TEXTURE_ID;
		textures[i].second = nullptr;
	}

	simple_depth_shader = App->resources->simple_depth_shader;
	if (simple_depth_shader != nullptr)
		simple_depth_shader->IncreaseReferences();
	else
		LOG_ENGINE("There was an error. Could not find the default shader");

	used_shader = App->resources->default_shader;
	if (used_shader != nullptr)
		used_shader->IncreaseReferences();
	else
		LOG_ENGINE("There was an error. Could not find the shadow shader");
}

ResourceMaterial::~ResourceMaterial()
{
	for (uint texType = 0; texType < (uint)TextureType::MAX; ++texType)
	{
		textures[texType].first = NO_TEXTURE_ID; 
		
		if (!App->IsQuiting())
		{
			if (textures[texType].second != nullptr)
			{
				textures[texType].second->DecreaseReferences(); 
			}
		}

		textures[texType].second = nullptr;

	}
}

bool ResourceMaterial::LoadMemory()
{
	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		if (textures[iter].first != NO_TEXTURE_ID)
		{
			if (textures[iter].second != nullptr)
				textures[iter].second->IncreaseReferences(); 
		}
	}

	return true;
}

void ResourceMaterial::FreeMemory()
{
	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		if (textures[iter].first != NO_TEXTURE_ID)
		{
			if (textures[iter].second != nullptr)
				textures[iter].second->DecreaseReferences();
		}
	}
}

void ResourceMaterial::OnSelected()
{
	LoadMemory();
}

void ResourceMaterial::OnDeselected()
{
	FreeMemory();
}

void ResourceMaterial::SaveResource()
{
	remove(path.c_str());

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, path.data());

	if (alien_value != nullptr && alien_object != nullptr) {
		JSONfilepack* alien = new JSONfilepack(path.data(), alien_object, alien_value);
		SaveMaterialValues(alien);
		delete alien;
	}

	CreateMetaData(ID);
}

bool ResourceMaterial::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	// META FILE

	std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data();

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, alien_path.data());

	if (alien_value != nullptr && alien_object != nullptr) {
		JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID).data());
		alien->FinishSave();
		delete alien;
	}

	// LIBRARY FILE 

	meta_data_path = LIBRARY_MATERIALS_FOLDER + std::to_string(ID) + ".material";

	std::string ext;
	App->file_system->SplitFilePath(path.data(), nullptr, nullptr, &ext);
	if (App->StringCmp(ext.data(), "material"))
	{
		App->file_system->Copy(path.data(), meta_data_path.data());
	}
	else
	{
		// ...?
	}

	App->resources->AddResource(this);
	return true;
}

bool ResourceMaterial::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	this->path = assets_file_path;

	// ---------------------------- READ META -----------------------------------
	std::string alien_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";

	this->name = App->file_system->GetBaseFileName(path.c_str());

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path.data(), object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;
	}

	// -------------------------- READ .MATERIAL -----------------------------------

	value = json_parse_file(path.data());
	object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* matFile = new JSONfilepack(alien_path.data(), object, value);

		ReadMaterialValues(matFile);

		delete matFile;
	}

	// -------------------------- READ LIBRARY -------------------------------------
	meta_data_path = LIBRARY_MATERIALS_FOLDER + std::to_string(ID) + ".material";

	if (!App->file_system->Exists(meta_data_path.data())) {
		App->file_system->Copy(path.data(), meta_data_path.data());
		App->resources->AddResource(this);
		return true;
	}

	struct stat fileMeta;
	struct stat fileAssets;

	if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
		if (fileAssets.st_mtime > fileMeta.st_mtime) {
			remove(meta_data_path.data());
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}

	App->resources->AddResource(this);

	return ret;
}

void ResourceMaterial::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data));

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path.data(), object, value);
		ReadMaterialValues(meta);
		delete meta;
	}

	App->resources->AddResource(this);
}

bool ResourceMaterial::DeleteMetaData()
{
	remove(meta_data_path.data());
	return true;
}

void ResourceMaterial::SaveMaterialValues(JSONfilepack* file)
{
	file->StartSave();

	file->SetString("Name", name.data());

	file->SetFloat4("Color", color);
	file->SetNumber("Smoothness", shaderInputs.standardShaderProperties.smoothness);
	file->SetNumber("Metalness", shaderInputs.standardShaderProperties.metalness);

	file->SetString("ShaderID", std::to_string(used_shader_ID).data());
	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		file->SetString(std::to_string(iter).data(), std::to_string(textures[iter].first).data());
	}

	file->FinishSave();
}

void ResourceMaterial::ReadMaterialValues(JSONfilepack* file)
{
	this->name = file->GetString("Name");

	color = file->GetFloat4("Color");
	shaderInputs.standardShaderProperties.smoothness = (float)file->GetNumber("Smoothness");
	shaderInputs.standardShaderProperties.metalness = (float)file->GetNumber("Metalness");

	SetShader((ResourceShader*)App->resources->GetResourceWithID(std::stoull(file->GetString("ShaderID"))));
	for (uint iter = 0; iter != (uint)TextureType::MAX; ++iter) {
		textures[iter].first = std::stoull(file->GetString(std::to_string(iter).data()));
		textures[iter].second = App->resources->GetTextureByID(textures[iter].first);
	}
}

void ResourceMaterial::ApplyMaterial()
{
	OPTICK_EVENT();

	// Bind the actual shader
	used_shader->Bind();

	if (textures[(uint)TextureType::DIFFUSE].first != NO_TEXTURE_ID && textures[(uint)TextureType::DIFFUSE].second != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[(uint)TextureType::DIFFUSE].second->id);
		used_shader->SetUniform1i("objectMaterial.diffuseTexture", 0);
		used_shader->SetUniform1i("objectMaterial.hasDiffuseTexture", 1);
	}
	else
	{
		used_shader->SetUniform1i("objectMaterial.hasDiffuseTexture", 0);
	}

	if (textures[(uint)TextureType::SPECULAR].first != NO_TEXTURE_ID && textures[(uint)TextureType::SPECULAR].second != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[(uint)TextureType::SPECULAR].second->id);
		used_shader->SetUniform1i("objectMaterial.specularMap", 1);
		used_shader->SetUniform1i("objectMaterial.hasSpecularMap", 1);
	}
	else
		used_shader->SetUniform1i("objectMaterial.hasSpecularMap", 0);

	if (textures[(uint)TextureType::NORMALS].first != NO_TEXTURE_ID && textures[(uint)TextureType::NORMALS].second != nullptr)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[(uint)TextureType::NORMALS].second->id);
		used_shader->SetUniform1i("objectMaterial.normalMap", 2);
		used_shader->SetUniform1i("objectMaterial.hasNormalMap", 1);
	}
	else
		used_shader->SetUniform1i("objectMaterial.hasNormalMap", 0);

	//default_shader->DrawShadows();

	// Update uniforms
	shaderInputs.standardShaderProperties.diffuse_color = color;
	shaderInputs.particleShaderProperties.color = color;
	shaderInputs.shieldFresnelShaderProperties.color = color;
	shaderInputs.shieldShaderProperties.color = float3(color.x, color.y, color.z);

	used_shader->UpdateUniforms(shaderInputs);

}

void ResourceMaterial::ApplyPreRenderShadows()
{
	// Bind the actual shader
	simple_depth_shader->Bind();

	if (!recive_shadow)
	{
		simple_depth_shader->has_shadow = true;
	}
	// Bind textures

	// Update uniforms
	shaderInputs.standardShaderProperties.diffuse_color = color;
	shaderInputs.particleShaderProperties.color = color;
	//simple_depth_shader->UpdateUniforms(shaderInputs);
}

void ResourceMaterial::UnbindMaterial()
{
	used_shader->Unbind();
	
	if (textures[(uint)TextureType::SPECULAR].first != NO_TEXTURE_ID)
	{	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (textures[(uint)TextureType::NORMALS].first != NO_TEXTURE_ID)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Leave active texture 0 by default, unbinded
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void ResourceMaterial::SetTexture(ResourceTexture* tex, TextureType texType)
{
	RemoveTexture(texType);

	if (tex == nullptr)
		return;

	tex->IncreaseReferences();
	textures[(uint)texType].first = tex->GetID();
	textures[(uint)texType].second = tex;
}

const ResourceTexture* ResourceMaterial::GetTexture(TextureType texType) const
{
	return textures[(uint)texType].second;
}

ResourceTexture* ResourceMaterial::GetTexture(TextureType texType)
{
	return textures[(uint)texType].second;
}

void ResourceMaterial::RemoveTexture(TextureType texType)
{
	if (textures[(uint)texType].first != NO_TEXTURE_ID)
	{
		if (textures[(uint)texType].second != nullptr)
			textures[(uint)texType].second->DecreaseReferences();

		textures[(uint)texType].first = NO_TEXTURE_ID;
		textures[(uint)texType].second = nullptr;
	}
}

bool ResourceMaterial::HasTexture(TextureType texType) const
{
	return textures[(int)texType].first != NO_TEXTURE_ID;
}

void ResourceMaterial::SetShader(ResourceShader* newShader)
{
	if (newShader == nullptr || newShader == used_shader)
		return;

	used_shader->DecreaseReferences();
	used_shader = newShader;
	used_shader_ID = used_shader->GetID();
	used_shader->IncreaseReferences();
}


void ResourceMaterial::DisplayMaterialOnInspector()
{
	if (ImGui::CollapsingHeader(GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SameLine();
		MaterialHeader();

		if (this == App->resources->default_material)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		ShaderSelectionHeader();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ShaderInputsSegment();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		TexturesSegment();

		if (ImGui::Button("Save Material"))
			SaveResource(); 

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (this == App->resources->default_material)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		if (change_texture_menu)
			TextureBrowser(selectedType);
	}
}

void ResourceMaterial::MaterialHeader()
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(std::string("Material References: " + std::to_string(references)).c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


void ResourceMaterial::ShaderSelectionHeader()
{
	std::vector<ResourceShader*> shadersList;
	App->resources->GetShaders(shadersList);
	selectedShader = used_shader->GetName();
	if (ImGui::BeginCombo("Selected Shader", selectedShader))
	{
		for (std::vector<ResourceShader*>::iterator iter = shadersList.begin(); iter != shadersList.end(); iter++)
		{
			bool is_selected = (App->StringCmp(selectedShader, (*iter)->GetName()));
			if (ImGui::Selectable((*iter)->GetName(), is_selected))
			{
				selectedShader = (*iter)->GetName();
				ResourceShader* newShader = App->resources->GetShaderByName(selectedShader);

				if (newShader != nullptr)
				{
					SetShader(newShader);
					LOG_ENGINE("Selected Shader %s\n", used_shader->GetName());
				}
				else
					is_selected = false;

			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ResourceMaterial::ShaderInputsSegment()
{

	switch (used_shader->GetShaderType())
	{
	case SHADER_TEMPLATE::DEFAULT: {//difusse
		//ImGui::ColorEdit3("Albedo", shaderInputs.standardShaderProperties.diffuse_color.ptr(), ImGuiColorEditFlags_Float);

		// Diffuse 
		ImGui::Text("Diffuse:");
		InputTexture(TextureType::DIFFUSE);
		ImGui::SameLine();
		ImGui::ColorEdit4("Albedo", color.ptr(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar /*|ImGuiColorEditFlags_NoInputs | */);

		// Specular 
		ImGui::Text("Specular:");
		InputTexture(TextureType::SPECULAR);
		ImGui::SameLine();
		float posX = ImGui::GetCursorPosX();
		ImGui::SliderFloat("Metalness", &shaderInputs.standardShaderProperties.metalness, 0.0f, 1.f);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(posX, -15));
		if(ImGui::Button("Reset Metalness")) shaderInputs.standardShaderProperties.metalness = DEFAULT_METALNESS;
		ImGui::SetCursorPosX(posX);
		ImGui::SliderFloat("Smoothness", &shaderInputs.standardShaderProperties.smoothness, 16.f, 128.f);
		ImGui::SetCursorPosX(posX);
		if (ImGui::Button("Reset Smoothness"))  shaderInputs.standardShaderProperties.smoothness = DEFAULT_SMOOTHNESS;

		// Normal Map
		ImGui::Text("Normal Map:");
		InputTexture(TextureType::NORMALS);

		break; }
	case SHADER_TEMPLATE::WAVE: {//wave
								
		ImGui::Spacing();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "velocity", (unsigned int)type); ImGui::SameLine();
		ImGui::InputFloat("## ", &shaderInputs.waveShaderProperties.mult_time, 0, 0, 2);

		ImGui::Spacing();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "amplitude", (unsigned int)type); ImGui::SameLine();
		ImGui::InputFloat("##", &shaderInputs.waveShaderProperties.amplitude, 0, 0, 2);

		break; }

	case SHADER_TEMPLATE::ILUMINATED: {
		ImGui::ColorEdit3("Albedo", shaderInputs.iluminatedShaderProperties.object_color.ptr(), ImGuiColorEditFlags_Float);
		break; }

	case SHADER_TEMPLATE::PARTICLE: {
		ImGui::Text("Texture:");
		ImGui::Spacing();

		InputTexture(TextureType::DIFFUSE);		

		ImGui::SameLine(120,15);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::ColorEdit3("Albedo",color.ptr(), ImGuiColorEditFlags_Float);
		break; }

	case SHADER_TEMPLATE::WATER: {//difusse
		//ImGui::ColorEdit3("Albedo", shaderInputs.standardShaderProperties.diffuse_color.ptr(), ImGuiColorEditFlags_Float);

		// Diffuse 
		ImGui::Text("Diffuse:");
		InputTexture(TextureType::DIFFUSE);
		ImGui::SameLine();
		ImGui::ColorEdit3("Albedo", color.ptr(), ImGuiColorEditFlags_Float /*|ImGuiColorEditFlags_NoInputs | */);

		// Specular 
		ImGui::Text("Specular:");
		InputTexture(TextureType::SPECULAR);
		ImGui::SameLine();
		float posX = ImGui::GetCursorPosX();
		ImGui::SliderFloat("Metalness", &shaderInputs.standardShaderProperties.metalness, 0.0f, 1.f);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(posX, -15));
		if (ImGui::Button("Reset Metalness")) shaderInputs.standardShaderProperties.metalness = DEFAULT_METALNESS;
		ImGui::SetCursorPosX(posX);
		ImGui::SliderFloat("Smoothness", &shaderInputs.standardShaderProperties.smoothness, 16.f, 128.f);
		ImGui::SetCursorPosX(posX);
		if (ImGui::Button("Reset Smoothness"))  shaderInputs.standardShaderProperties.smoothness = DEFAULT_SMOOTHNESS;

		// Normal Map
		ImGui::Text("Normal Map:");
		InputTexture(TextureType::NORMALS);
		break;}
		
	case SHADER_TEMPLATE::SHIELD: {
		ImGui::SliderFloat3("Hit Position", (float*)&shaderInputs.shieldShaderProperties.hit_position, -1.0f, 1.0f);
		ImGui::Spacing();
		ImGui::ColorEdit3("Albedo", color.ptr(), ImGuiColorEditFlags_Float);
		break; }

	case SHADER_TEMPLATE::SHIELD_FRESNEL: {
		ImGui::ColorEdit4("Albedo", color.ptr(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar /*|ImGuiColorEditFlags_NoInputs | */);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Strength", (unsigned int)type); ImGui::SameLine();
		ImGui::SliderFloat("##strength", &shaderInputs.shieldFresnelShaderProperties.shieldStrength, -1.0f, 1.0f);
		//ImGui::InputFloat("##strength", &shaderInputs.shieldFresnelShaderProperties.shieldStrength, 0.1, 1, 2);
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Cooldown", (unsigned int)type); ImGui::SameLine();
		//ImGui::InputFloat("##cooldown", &shaderInputs.shieldFresnelShaderProperties.shieldCooldown, 0.1,1, 2);
		ImGui::SliderFloat("##cooldown", &shaderInputs.shieldFresnelShaderProperties.shieldCooldown, 0.0f, 1.0f);
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Fresnel", (unsigned int)type); ImGui::SameLine();
		//ImGui::InputFloat("##fresnel", &shaderInputs.shieldFresnelShaderProperties.fresnel_exponent, 0.1, 1, 2);
		ImGui::SliderFloat("##fresnel", &shaderInputs.shieldFresnelShaderProperties.fresnel_exponent, 0.0f, 1.0f);
		
		/*ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Num Hits", (unsigned int)type); ImGui::SameLine();
		ImGui::InputInt("##numhits", &shaderInputs.shieldFresnelShaderProperties.numHits);*/

		break; }

	default:
		LOG_ENGINE("We currently don't support editing this type of uniform...");
		break;
	}
	ImGui::Spacing();

}

void ResourceMaterial::InputTexture(TextureType texType)
{
	ImGui::ImageButton((ImTextureID)App->resources->GetTextureidByID(textures[(uint)texType].first), ImVec2(30, 30));
	if (ImGui::BeginDragDropTarget() && this != App->resources->default_material) {
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
						SetTexture(texture, texType);

						// Save files when modifying material's textures
						SaveResource();
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}	

	ImGui::SameLine();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::PushID((int)texType);
	if (ImGui::RadioButton("###", false))
	{
		RemoveTexture(texType);

		// Save files when modifying material's textures
		SaveResource();

		// On hold to revise references
		/*change_texture_menu = true;
		selectedType = texType;*/
	}
	ImGui::PopID();
}

void ResourceMaterial::TexturesSegment()
{
	//static ResourceTexture* selected_texture = nullptr;
	//if (texture != nullptr)
	//{	
	//	ImGui::Text("Texture Information: %s", std::to_string(texture->GetID()).c_str());

	//	//ImGui::SameLine(140, 15);
	//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
	//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
	//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
	//	if (ImGui::Button("Delete", { 60,20 })) {
	//		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
	//		RemoveTexture();
	//		ImGui::PopStyleColor();
	//		ImGui::PopStyleColor();
	//		ImGui::PopStyleColor();
	//		return;
	//	}

	//	ImGui::PopStyleColor();
	//	ImGui::PopStyleColor();
	//	ImGui::PopStyleColor();

	//	static bool check;
	//	check = textureActivated;
	//	if (ImGui::Checkbox("Texture Active", &check)) {
	//		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
	//		textureActivated = check;
	//	}
	//	ImGui::SameLine(220, 15);
	//	ImGui::Spacing();
	//	ImGui::Text("Texture References: %i", texture->references);
	//	ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->width);
	//	ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->height);
	//	ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", texture->GetAssetsPath());

	//	ImGui::Image((ImTextureID)texture->id, { ImGui::GetWindowWidth() ,ImGui::GetWindowWidth() });
	//	ImGui::Spacing();
	//}
	//else {
	//	ImGui::SameLine(220, 15);
	//	if (ImGui::Button("Add Texture", { 120,20 })) {
	//		/*change_texture_menu = true;
	//		selected_texture = texture;*/
	//	}
	//}

	 //Textures window preview 

	/*if (change_texture_menu) {
		ImGui::OpenPopup("Textures Loaded");
		ImGui::SetNextWindowSize({ 522,570 });
		if (ImGui::BeginPopupModal("Textures Loaded", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::Spacing();
			ImGui::NewLine();
			ImGui::SameLine(190);
			ImGui::Text("Texture Selected");
			ImGui::Text("");
			ImGui::SameLine(170);
			if (selected_texture != nullptr) {
				ImGui::Image((ImTextureID)selected_texture->id, { 150,150 });
				ImGui::Spacing();
				ImGui::Text("");
				ImGui::SameLine(150);
				ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->width);
				ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->height);
				ImGui::Text("");
				ImGui::SameLine(112);
				ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", selected_texture->GetAssetsPath());
			}
			ImGui::Spacing();

			if (ImGui::BeginChild("##TexturesSelectorChild", { 492,285 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
				ImGui::Columns(3, 0, false);
				ImGui::SetColumnWidth(0, 156);
				ImGui::SetColumnWidth(1, 156);
				ImGui::SetColumnWidth(2, 156);

				std::vector<Resource*>::iterator item = App->resources->resources.begin();
				for (; item != App->resources->resources.end(); ++item) {
					if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
						ImGui::ImageButton((ImTextureID)static_cast<ResourceTexture*>(*item)->id, { 140,140 });
						if (ImGui::IsItemClicked()) {
							selected_texture = static_cast<ResourceTexture*>(*item);
						}
						ImGui::NextColumn();
					}
				}

				ImGui::EndChild();
			}
			ImGui::Spacing();
			ImGui::Text("");
			ImGui::SameLine(377);
			if (ImGui::Button("Apply", { 120,20 })) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				texture = selected_texture;
				selected_texture = nullptr;
				change_texture_menu = false;
			}
			ImGui::SameLine(237);
			if (ImGui::Button("Cancel", { 120,20 })) {
				selected_texture = nullptr;
				change_texture_menu = false;
			}
			ImGui::EndPopup();
		}
	}*/
}

void ResourceMaterial::TextureBrowser(TextureType selectedType)
{

	ImGui::OpenPopup("Textures Loaded");
	ImGui::SetNextWindowSize({ 522,585 });

	if (ImGui::BeginPopupModal("Textures Loaded", &change_texture_menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
		ImGui::Spacing();
		ImGui::NewLine();
		ImGui::SameLine(190);
		ImGui::Text("Texture Selected");
		ImGui::Text("");
		ImGui::SameLine(170);

		if (selected_texture != nullptr) {
			ImGui::Image((ImTextureID)selected_texture->id, { 150,150 });
			ImGui::Spacing();
			ImGui::Text("");
			ImGui::SameLine(150);

			ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->width);
			ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->height);
			ImGui::Text("");
			ImGui::SameLine(150);
			ImGui::Text("References:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->references);
			ImGui::Text("");
			ImGui::SameLine(112);
			ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", selected_texture->GetAssetsPath());
		}
		ImGui::Spacing();

		if (ImGui::BeginChild("##TexturesSelectorChild", { 492,285 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
			ImGui::Columns(3, 0, false);
			ImGui::SetColumnWidth(0, 156);
			ImGui::SetColumnWidth(1, 156);
			ImGui::SetColumnWidth(2, 156);

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {

					if ((*item)->NeedToLoad())
						(*item)->IncreaseReferences();

					ImGui::ImageButton((ImTextureID)static_cast<ResourceTexture*>(*item)->id, { 140,140 });
					if (ImGui::IsItemClicked()) {
						selected_texture = static_cast<ResourceTexture*>(*item);
					}
					ImGui::NextColumn();
				}
			}

			ImGui::EndChild();
		}
		ImGui::Spacing();
		ImGui::Text("");
		ImGui::SameLine(377);
		if (ImGui::Button("Apply", { 120,20 })) {
			
			SetTexture(selected_texture, selectedType);

			selected_texture = nullptr;
			change_texture_menu = false;

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {

					if (*item != GetTexture(selectedType))
						(*item)->DecreaseReferences();
				}
			}
		}
		ImGui::SameLine(237);
		if (ImGui::Button("Cancel", { 120,20 })) {

			selected_texture = nullptr;
			change_texture_menu = false;

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
					(*item)->DecreaseReferences();
				}
			}
		}
		ImGui::SameLine(100);
		if (ImGui::Button("None", { 120,20 })) {

			RemoveTexture(selectedType);
			selected_texture = nullptr;
			change_texture_menu = false;

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {

					if (*item != GetTexture(selectedType))
						(*item)->DecreaseReferences();
				}
			}
		}
		ImGui::EndPopup();
	}














}



