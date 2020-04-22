#include "ResourceAudio.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleAudio.h"

#include <ctime>
#include "mmgr/mmgr.h"

ResourceAudio::ResourceAudio() : Resource() {
	type = ResourceType::RESOURCE_AUDIOBANK;
}

ResourceAudio::~ResourceAudio()
{
}

bool ResourceAudio::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	App->file_system->NormalizePath(path);
	name = App->file_system->GetBaseFileName(path.data());

	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	json_serialize_to_file_pretty(value, meta_path.data());

	if (value != nullptr && json_object != nullptr) {

		JSONfilepack* file = new JSONfilepack(meta_path.data(), json_object, value);
		file->StartSave();
		file->SetString("Meta.ID", std::to_string(ID).data());
		file->SetNumber("Meta.timestamp", std::time(nullptr));
		file->FinishSave();
		delete file;
	}

	meta_data_path = LIBRARY_AUDIO_FOLDER + std::to_string(ID) + ".bnk";

	App->file_system->Copy(path.c_str(), meta_data_path.c_str());

	App->resources->AddResource(this);

	return true;
}

bool ResourceAudio::ReadBaseInfo(const char* assets_file_path)
{
	path = std::string(assets_file_path);
	name = App->file_system->GetBaseFileName(path.data());

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	ID = App->resources->GetIDFromAlienPath(meta_path.data());

	if (ID != 0) {
		meta_data_path = LIBRARY_AUDIO_FOLDER + std::to_string(ID) + ".bnk";

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

void ResourceAudio::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;
	ID = std::stoul(App->file_system->GetBaseFileName(meta_data_path.data()));

	Bank* bk = new Bank();
	bk->id = (u32)ID;
	bk->name.assign(std::to_string(bk->id));
	
	App->audio->AddBank(bk);

	App->resources->AddResource(this);
}
