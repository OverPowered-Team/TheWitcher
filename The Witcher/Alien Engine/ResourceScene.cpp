#include "ResourceScene.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "JSONfilepack.h"
#include "ModuleResources.h"
#include "mmgr/mmgr.h"

ResourceScene::ResourceScene() : Resource()
{
	type = ResourceType::RESOURCE_SCENE;
}

ResourceScene::~ResourceScene()
{
}

bool ResourceScene::CreateMetaData(const u64& force_id)
{
	if (force_id != 0) {
		ID = force_id;
	}
	else {
		ID = App->resources->GetRandomID();
	}

	App->file_system->NormalizePath(path);
	name = App->file_system->GetBaseFileName(path.data());

	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);
	json_serialize_to_file_pretty(value, path.data());

	if (value != nullptr && json_object != nullptr) {
		JSONfilepack* file = new JSONfilepack(path.data(), json_object, value);
		file->StartSave();

		file->SetString("Scene.Name", name.data());
		file->InitNewArray("Scene.GameObjects");
		file->FinishSave();
		delete file;
	}


	JSON_Value* value2 = json_value_init_object();
	JSON_Object* json_object2 = json_value_get_object(value2);

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	json_serialize_to_file_pretty(value2, meta_path.data());

	if (value2 != nullptr && json_object2 != nullptr) {

		JSONfilepack* file = new JSONfilepack(meta_path.data(), json_object2, value2);
		file->StartSave();
		file->SetString("Meta.ID", std::to_string(ID).data());
		file->FinishSave();
		delete file;
	}

	meta_data_path = LIBRARY_SCENES_FOLDER + std::to_string(ID) + ".alienScene";
	App->file_system->Copy(path.data(), meta_data_path.data());
	App->resources->AddResource(this);
	return true;
}

bool ResourceScene::ReadBaseInfo(const char* assets_file_path)
{
	path = std::string(assets_file_path);
	name = App->file_system->GetBaseFileName(path.data());

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	ID = App->resources->GetIDFromAlienPath(meta_path.data());

	if (ID != 0) {
		meta_data_path = LIBRARY_SCENES_FOLDER + std::to_string(ID) + ".alienScene";

		if (!App->file_system->Exists(meta_data_path.data())) {
			App->file_system->Copy(assets_file_path, meta_data_path.data());
		}
		else {
			struct stat fileMeta;
			struct stat fileAssets;
			if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
				if (fileAssets.st_mtime > fileMeta.st_mtime) {
					remove(meta_data_path.data());
					App->file_system->Copy(path.data(), meta_data_path.data());
				}
			}
		}
		App->resources->AddResource(this);
		return true;
	}
	return false;
}

void ResourceScene::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path.data(), object, value);
		name = meta->GetString("Scene.Name");
		delete meta;
	}

	App->resources->AddResource(this);
}

bool ResourceScene::DeleteMetaData()
{
	remove(meta_data_path.data());
	return true;
}
