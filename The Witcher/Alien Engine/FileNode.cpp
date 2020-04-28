#include "FileNode.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ResourceModel.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourcePrefab.h"
#include "ResourceScene.h"
#include "ResourceAnimatorController.h"
#include "ResourceMaterial.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <fstream>
#include "Prefab.h"
#include "mmgr/mmgr.h"

FileNode::FileNode()
{
}

FileNode::FileNode(std::string name, bool is_file, FileNode* parent)
{
	this->name = name;
	this->is_file = is_file;
	this->parent = parent;

	std::string previous_names;
	App->file_system->GetPreviousNames(previous_names, parent);
	path = previous_names;

	// set icon
	SetIcon();
}

FileNode::FileNode(const std::string& path, const std::string& name, bool is_file, FileNode* parent)
{
	this->name = name;
	this->is_file = is_file;
	this->parent = parent;
	this->path = path;

	// set icon
	SetIcon();
}

FileNode::~FileNode()
{
	DeleteChildren();
}

void FileNode::DeleteChildren()
{
	std::vector<FileNode*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->DeleteChildren();
			delete (*item);
			*item = nullptr;
		}
	}
	children.clear();
}

void FileNode::DeleteNodeData(bool delete_folder)
{
	if (is_file) {
		std::string hole_path = std::string(path + name).data();

		std::string meta_path;

		meta_path = App->file_system->GetPathWithoutExtension(hole_path.data()) + "_meta.alien";

		u64 ID = App->resources->GetIDFromAlienPath(meta_path.data());
		if (ID != 0) {
			remove(meta_path.data());

			Resource* resource_to_delete = App->resources->GetResourceWithID(ID);
			if (resource_to_delete != nullptr) {
				resource_to_delete->DeleteMetaData();
				auto item = App->resources->resources.begin();
				for (; item != App->resources->resources.end(); ++item) {
					if (*item == resource_to_delete) {
						delete* item;
						*item = nullptr;
						App->resources->resources.erase(item);
						break;
					}
				}
			}
		}
		remove(hole_path.data());
	}
	else {
		std::vector<FileNode*>::iterator item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr)
				(*item)->DeleteNodeData(false);
		}

		if (delete_folder) {			
			if (std::experimental::filesystem::remove_all(path.data())) {
				LOG_ENGINE("Folder removed successfully %s", path.data());
			}
			else {
				LOG_ENGINE("Could not remove folder %s", path.data());
			}
		}
	}
}

void FileNode::ResetPaths()
{
	switch (type)
	{
	case FileDropType::MODEL3D: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceModel* model = (ResourceModel*)App->resources->GetResourceWithID(ID);
		if (model != nullptr) {
			model->SetAssetsPath(std::string(this->path + name).data());
		}
		break; }
	case FileDropType::TEXTURE: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if (texture != nullptr) {
			texture->SetAssetsPath(std::string(this->path + name).data());
			texture->SetName(App->file_system->GetBaseFileName(name.c_str()).c_str());
		}
		break; }
	case FileDropType::SHADER: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceShader* shader = (ResourceShader*)App->resources->GetResourceWithID(ID);
		if (shader != nullptr) {
			shader->SetAssetsPath(std::string(this->path + name).data());
			shader->SetName(App->file_system->GetBaseFileName(name.c_str()).c_str());
		}
		break; }
	case FileDropType::MATERIAL: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceMaterial* material = (ResourceMaterial*)App->resources->GetResourceWithID(ID);
		if (material != nullptr) {
			material->SetAssetsPath(std::string(this->path + name).data());
			material->SetName(App->file_system->GetBaseFileName(name.c_str()).c_str());
		}
		break; }
	case FileDropType::ANIM_CONTROLLER: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceAnimatorController* anim_ctrl = (ResourceAnimatorController*)App->resources->GetResourceWithID(ID);
		if (anim_ctrl != nullptr) {
			anim_ctrl->SetAssetsPath(std::string(this->path + name).data());
			anim_ctrl->name = App->file_system->GetBaseFileName(std::string(this->path + name).data()).data();
		}
		break; }
	case FileDropType::PREFAB: {
		std::string path = App->file_system->GetPathWithoutExtension(this->path + name);
		path += "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(ID);
		if (prefab != nullptr) {
			prefab->SetAssetsPath(std::string(this->path + name).data());
			prefab->SetName(App->file_system->GetBaseFileName(std::string(this->path + name).data()).data());
			auto item = prefab->prefab_references.begin();
			for (; item != prefab->prefab_references.end(); ++item) {
				if (*item != nullptr) {
					(*item)->prefab_name = std::string(prefab->name);
				}
			}

			JSONfilepack* pack = JSONfilepack::GetJSON(prefab->path.data());
			pack->StartSave();
			pack->SetString("Name", prefab->name.data());
			pack->FinishSave();
			delete pack;
			remove(prefab->GetLibraryPath());
			App->file_system->Copy(prefab->GetAssetsPath(), prefab->GetLibraryPath());
		}
		break; }
	case FileDropType::SCENE: {
		static char curr_dir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, curr_dir);
		std::string meta_name = App->file_system->GetPathWithoutExtension(path + name) + "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(meta_name.data());
		if (ID != 0) {
			ResourceScene* scene = (ResourceScene*)App->resources->GetResourceWithID(ID);
			scene->SetAssetsPath(std::string(path + name).data());
			if (scene != nullptr) {
				std::ifstream file(scene->GetAssetsPath());
				std::string file_str;
				std::string new_name = App->file_system->GetBaseFileName(name.data());
				if (file.is_open()) {
					std::string line;
					bool done = false;
					while (std::getline(file, line)) {
						if (!done && line.find(scene->GetName()) != std::string::npos) {
							line.replace(line.find(scene->GetName()), std::string(scene->GetName()).size(), new_name.data());
							done = true;
						}
						if (file_str.empty()) {
							file_str = line;
						}
						else {
							file_str += std::string("\n") + line;
						}
					}
					file.close();
				}
				remove(scene->GetAssetsPath());
				remove(scene->GetLibraryPath());
				App->file_system->Save(scene->GetAssetsPath(), file_str.data(), file_str.size());
				App->file_system->Save(scene->GetLibraryPath(), file_str.data(), file_str.size());
				scene->SetName(new_name.data());
			}
		}
		break; }
	default: {
		LOG_ENGINE("Type in reset paths not added");
		break; }
	}
}

FileNode* FileNode::FindChildrenByPath(const std::string& path)
{
	FileNode* ret = nullptr;
	std::vector<FileNode*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (ret != nullptr)
			break;
		if (*item != nullptr) {
			if (App->StringCmp((*item)->path.data(), path.data()))
				return (*item);
			else
				ret = (*item)->FindChildrenByPath(path);
		}
	}
	return ret;
}

void FileNode::RemoveResourceOfGameObjects()
{
	if (is_file) {
		//SDL_assert((uint)FileDropType::UNKNOWN == 10);
		switch (type) {
		case FileDropType::SCENE:
			static char curr_dir[MAX_PATH];
			GetCurrentDirectoryA(MAX_PATH, curr_dir);
			if (!App->objects->current_scenes.empty()) {
				for (auto item = App->objects->current_scenes.begin(); item != App->objects->current_scenes.end(); ++item) {
					if (App->StringCmp(App->file_system->GetBaseFileName(name.data()).data(), (*item)->GetName())) {
						App->objects->current_scenes.erase(item);
						break;
					}
				}
			}
			break;
		case FileDropType::SCRIPT:
			// TODO:
			break;
		case FileDropType::ANIM_CONTROLLER: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceAnimatorController* anim_controller_to_delete = (ResourceAnimatorController*)App->resources->GetResourceWithID(ID);
			if (anim_controller_to_delete != nullptr) {
				App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_ANIMATOR_CONTROLLER, (Resource*)anim_controller_to_delete);
			}
			break; }
		case FileDropType::TEXTURE: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceTexture* texture_to_delete = (ResourceTexture*)App->resources->GetResourceWithID(ID);
			if (texture_to_delete != nullptr) {
				App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_TEXTURE, (Resource*)texture_to_delete);
			}
			break; }
		case FileDropType::SHADER: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceShader* shader_to_delete = (ResourceShader*)App->resources->GetResourceWithID(ID);
			if (shader_to_delete != nullptr) {
				App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_SHADER, (Resource*)shader_to_delete);
			}
			break; }
		case FileDropType::MATERIAL: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceMaterial* material_to_delete = (ResourceMaterial*)App->resources->GetResourceWithID(ID);
			if (material_to_delete != nullptr) {
				App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_MATERIAL, (Resource*)material_to_delete);
			}
			break; }
		case FileDropType::MODEL3D: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceModel* model_to_delete = (ResourceModel*)App->resources->GetResourceWithID(ID);
			if (model_to_delete != nullptr) {
				for (uint i = 0; i < model_to_delete->meshes_attached.size(); ++i) {
					if (model_to_delete->meshes_attached[i] != nullptr) {
						App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_MESH, (Resource*)model_to_delete->meshes_attached[i]);
					}
				}
			}
			break; }
		case FileDropType::PREFAB: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(ID);
			if (prefab != nullptr) {
				auto item = prefab->prefab_references.begin();
				for (; item != prefab->prefab_references.end(); ++item) {
					if (*item != nullptr) {
						(*item)->prefabID = 0;
						(*item)->prefab_name.clear();
					}
				}
			}
			break; }
		case FileDropType::FONT: {
			std::string path_ = App->file_system->GetPathWithoutExtension(path + name);
			path_ += "_meta.alien";
			u64 ID = App->resources->GetIDFromAlienPath(path_.data());
			ResourceFont* font_to_delete = (ResourceFont*)App->resources->GetResourceWithID(ID);
			if (font_to_delete != nullptr) {
				App->objects->GetRoot(true)->SearchResourceToDelete(ResourceType::RESOURCE_FONT, (Resource*)font_to_delete);
			}
			break; }
		}
	}
	else {
		for (uint i = 0; i < children.size(); ++i) {
			children[i]->RemoveResourceOfGameObjects();
		}
	}
}

void FileNode::ResetChildrenPath()
{
	for (auto item = children.begin(); item != children.end(); ++item) {
		(*item)->ResetPaths();
		(*item)->ResetChildrenPath();
	}
}

void FileNode::SetIcon()
{
	if (is_file) {
		std::string extension;
		App->file_system->SplitFilePath(std::string(path + name).data(), nullptr, nullptr, &extension);

		if (App->StringCmp(extension.data(), "jpg")) {
			icon = App->resources->icons.jpg_file;
			type = FileDropType::TEXTURE;
		}
		else if (App->StringCmp(extension.data(), "dds")) {
			icon = App->resources->icons.dds_file;
			type = FileDropType::TEXTURE;
		}
		else if (App->StringCmp(extension.data(), "png")) {
			icon = App->resources->icons.png_file;
			type = FileDropType::TEXTURE;
		}
		else if (App->StringCmp(extension.data(), "tga")) {
			icon = App->resources->icons.tga_file;
			type = FileDropType::TEXTURE;
		}
		else if (App->StringCmp(extension.data(), "shader")) {
			icon = App->resources->icons.shader_file;
			type = FileDropType::SHADER;
		}
		else if (App->StringCmp(extension.data(), "material")) {
			icon = App->resources->icons.model;
			type = FileDropType::MATERIAL;
		}
		else if (App->StringCmp(extension.data(), "fbx") || App->StringCmp(extension.data(), "dae")) {
			icon = App->resources->icons.model;
			type = FileDropType::MODEL3D;
		}
		else if (App->StringCmp(extension.data(), "ttf")) {
			icon = App->resources->icons.model;
			type = FileDropType::FONT;
		}
		else if (App->StringCmp(extension.data(), "otf")) {
			icon = App->resources->icons.model;
			type = FileDropType::FONT;
		}
		else if (App->StringCmp(extension.data(), "alienScene")) {
			icon = App->resources->icons.scene_file;
			type = FileDropType::SCENE;
		}
		else if (App->StringCmp(extension.data(), "alienPrefab")) {
			icon = App->resources->icons.prefab_icon;
			type = FileDropType::PREFAB;
		}
		else if (App->StringCmp(extension.data(), "alienScript")) {
			icon = App->resources->icons.script_file;
			type = FileDropType::SCRIPT;
		}
		else if (App->StringCmp(extension.data(), "animController")) {
			icon = App->resources->icons.model;
			type = FileDropType::ANIM_CONTROLLER;
		}
		else if (App->StringCmp(extension.data(), "alienParticles")) {
			icon = App->resources->icons.model;
			type = FileDropType::PARTICLES;
		}
		else if (App->StringCmp(extension.data(), "fnt")) {
			icon = App->resources->icons.model;
			type = FileDropType::FONT;
		}
		else {
			// TODO: fer un icon que sigui unknown
			icon = App->resources->icons.model;
		}
	}
	else {
		icon = App->resources->icons.folder;
		type = FileDropType::FOLDER;
	}
}
