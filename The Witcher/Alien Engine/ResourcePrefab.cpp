#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentLightDirectional.h"
#include "ComponentTransform.h"
#include "PanelHierarchy.h"
#include "Prefab.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "ComponentAnimator.h"
#include "ModuleCamera3D.h"
#include "ComponentUI.h"
#include "Time.h"
#include "ComponentDeformableMesh.h"
#include "ComponentRigidBody.h"
#include "ComponentCharacterController.h"

#include "mmgr/mmgr.h"

ResourcePrefab::ResourcePrefab() : Resource()
{
	type = ResourceType::RESOURCE_PREFAB;
}

ResourcePrefab::~ResourcePrefab()
{
	prefab_references.clear();
}

bool ResourcePrefab::CreateMetaData(GameObject* object, const char* folder, u64 force_id)
{
	App->objects->is_saving_prefab = true;
	std::vector<std::string> files;
	std::vector<std::string> dir;
	if (folder == nullptr) {
		path = std::string(ASSETS_PREFAB_FOLDER + std::string(object->GetName()) + ".alienPrefab");
		App->file_system->DiscoverFiles(ASSETS_PREFAB_FOLDER, files, dir);
	}
	else {
		path = std::string(std::string(folder) + std::string(object->GetName()) + ".alienPrefab");
		App->file_system->DiscoverFiles(folder, files, dir);
	}

	if (force_id != 0) {
		ID = force_id;
	}
	else {
		ID = App->resources->GetRandomID();
	}

	if (!files.empty()) {
		uint num_file = 0;
		for (uint i = 0; i < files.size(); ++i) {
			if (App->StringCmp(files[i].data(), App->file_system->GetBaseFileNameWithExtension(path.data()).data())) {
				++num_file;
				if (folder == nullptr) {
					path = std::string(ASSETS_PREFAB_FOLDER + std::string(object->GetName()) + " (" + std::to_string(num_file) + ")" + ".alienPrefab");
				}
				else {
					path = std::string(std::string(folder) + std::string(object->GetName()) + " (" + std::to_string(num_file) + ")" + ".alienPrefab");
				}
				i = -1;
			}
		}
	}

	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);
	json_serialize_to_file_pretty(value, path.data());

	if (value != nullptr && json_object != nullptr) {
		// save ID in assets
		JSONfilepack* prefab_scene = new JSONfilepack(path.data(), json_object, value);
		prefab_scene->StartSave();

		SetName(App->file_system->GetBaseFileName(path.data()).data());
		prefab_scene->SetString("Name", name.data());
		// save prefab in library
		meta_data_path = path;

		object->SetPrefab(ID);

		JSONArraypack* game_objects = prefab_scene->InitNewArray("Prefab.GameObjects");

		game_objects->SetAnotherNode();

		App->objects->SaveGameObject(object, game_objects, 1);

		prefab_scene->FinishSave();
		delete prefab_scene;

		App->resources->AddResource(this);

		std::string meta_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
		JSON_Value* meta_value = json_value_init_object();
		JSON_Object* meta_object = json_value_get_object(meta_value);
		json_serialize_to_file_pretty(meta_value, meta_path.data());

		if (meta_value != nullptr && meta_object != nullptr) {
			JSONfilepack* meta = new JSONfilepack(meta_path.data(), meta_object, meta_value);
			meta->StartSave();
			meta->SetString("Meta.ID", std::to_string(ID).data());
			meta->FinishSave();
			delete meta;

			meta_data_path = LIBRARY_PREFABS_FOLDER + std::to_string(ID) + ".alienPrefab";
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}
	else {
		LOG_ENGINE("Could not load scene, fail when creating the file");
	}
	App->objects->is_saving_prefab = false;
	return true;
}

bool ResourcePrefab::ReadBaseInfo(const char* assets_file_path)
{
	path = std::string(assets_file_path);

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");
	if (App->file_system->Exists(meta_path.data())) {
		ID = App->resources->GetIDFromAlienPath(meta_path.data());
	}
	else {
		ID = App->resources->GetRandomID();
		JSON_Value* value2 = json_value_init_object();
		JSON_Object* json_object2 = json_value_get_object(value2);
		json_serialize_to_file_pretty(value2, meta_path.data());

		if (value2 != nullptr && json_object2 != nullptr) {

			JSONfilepack* file = new JSONfilepack(meta_path.data(), json_object2, value2);
			file->StartSave();
			file->SetString("Meta.ID", std::to_string(ID).data());
			file->FinishSave();
			delete file;
		}
	}

	if (ID != 0) {
		meta_data_path = LIBRARY_PREFABS_FOLDER + std::to_string(ID) + ".alienPrefab";
		if (!App->file_system->Exists(meta_data_path.data())) {
			App->file_system->Copy(path.data(), meta_data_path.data());
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

		JSONfilepack* pack = JSONfilepack::GetJSON(path.data());
		try {
			name = pack->GetString("Name");
		}
		catch (...) {
			pack->StartSave();
			pack->SetString("Name", App->file_system->GetBaseFileName(path.data()).data());
			pack->FinishSave();
			remove(GetLibraryPath());
			App->file_system->Copy(GetAssetsPath(), GetLibraryPath());
		}
		delete pack;
		
		App->resources->AddResource(this);
	}

	return true;
}

void ResourcePrefab::ReadLibrary(const char* meta_data)
{
	meta_data_path = std::string(meta_data);

	JSONfilepack* pack = JSONfilepack::GetJSON(meta_data_path.data());
	try {
		name = pack->GetString("Name");
	}
	catch (...) {

	}
	delete pack;

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	App->resources->AddResource(this);
}

bool ResourcePrefab::DeleteMetaData()
{
	if (App->objects->prefab_scene && App->objects->prefab_opened == this) {
		App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
		App->objects->prefab_opened = nullptr;
		App->objects->prefab_scene = false;
		App->objects->enable_instancies = true;
		App->objects->SwapReturnZ(true, true);
	}
	remove(meta_data_path.data());
	App->objects->GetRoot(true)->UnpackAllPrefabsOf(ID);
	return true;
}

void ResourcePrefab::Save(GameObject* prefab_root)
{
	App->objects->is_saving_prefab = true;
	remove(meta_data_path.data());
	remove(path.data());
	JSON_Value* prefab_value = json_value_init_object();
	JSON_Object* prefab_object = json_value_get_object(prefab_value);
	json_serialize_to_file_pretty(prefab_value, path.data());
	prefab_root->SetPrefab(ID);
	if (prefab_value != nullptr && prefab_object != nullptr) {
		JSONfilepack* prefab = new JSONfilepack(path.data(), prefab_object, prefab_value);
		prefab->StartSave();
		JSONArraypack* game_objects = prefab->InitNewArray("Prefab.GameObjects");

		game_objects->SetAnotherNode();

		App->objects->SaveGameObject(prefab_root, game_objects, 1);

		prefab->FinishSave();

		App->file_system->Copy(path.data(), meta_data_path.data());
		delete prefab;
	}
	if (App->objects->prefab_scene) {
		App->objects->prefab_opened = nullptr;
		App->objects->prefab_scene = false;
		App->objects->SwapReturnZ(true, true);
		App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
		App->objects->enable_instancies = true;
		remove("Library/save_prefab_scene.alienScene");
	}
	App->objects->is_saving_prefab = false;
}

void ResourcePrefab::OpenPrefabScene()
{
	if (Time::IsInGameState()) {
		App->ui->panel_hierarchy->popup_no_open_prefab = true;
		return;
	}
	if (App->objects->prefab_scene) {
		return;
	}
	App->objects->prefab_opened = this;
	App->objects->enable_instancies = false;
	App->objects->SwapReturnZ(false, false);
	App->objects->prefab_scene = true;
	App->objects->SaveScene(nullptr, "Library/save_prefab_scene.alienScene");
	App->objects->DeselectObjects();
	App->objects->CreateRoot();
	ConvertToGameObjects(App->objects->GetGlobalRoot());
}

GameObject* ResourcePrefab::ConvertToGameObjects(GameObject* parent, int list_num, float3 pos, bool check_childrens, bool set_selected)
{
	if (!set_selected) {
		App->objects->inPrefabCreation = true;
	}
	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* prefab = new JSONfilepack(meta_data_path.data(), object, value);

		JSONArraypack* game_objects = prefab->GetArray("Prefab.GameObjects");

		std::vector<GameObject*> objects_created;
		bool is_first = true;
		for (uint i = 0; i < game_objects->GetArraySize(); ++i) {
			GameObject* obj = new GameObject(true);
			u64 parentID = std::stoull(game_objects->GetString("ParentID"));
			if (!is_first) {
				std::vector<GameObject*>::iterator objects = objects_created.begin();
				for (; objects != objects_created.end(); ++objects) {
					if ((*objects)->ID == parentID) {
						obj->LoadObject(game_objects, *objects);
						break;
					}
				}
			}
			else {
				obj->LoadObject(game_objects, parent);
				is_first = false;
			}
			objects_created.push_back(obj);
			game_objects->GetAnotherNode();
		}
		GameObject* obj = parent->children.back();

		if (!App->objects->to_add.empty()) {
			auto item = App->objects->to_add.begin();
			for (; item != App->objects->to_add.end(); ++item) {
				GameObject* found = obj->GetGameObjectByIDReverse(((*item).first));
				if (found != nullptr) {
					*(*item).second = found;
				}
			}
		}

		if (list_num != -1) {
			parent->children.pop_back();
			parent->children.insert(parent->children.begin() + list_num, obj);
		}

		for each (GameObject * obj2 in objects_created) //not sure where to place this, need to link skeletons to meshes after all go's have been created
		{
			ComponentDeformableMesh* def_mesh = obj2->GetComponent<ComponentDeformableMesh>();
			if (def_mesh) {
				if (def_mesh->rootID != 0) {
					def_mesh->root_bone = obj->GetGameObjectByID(def_mesh->rootID);
					if (def_mesh->root_bone != nullptr)
						def_mesh->AttachSkeleton(def_mesh->root_bone->transform);
				}
			}
			ComponentAnimator* animator = obj2->GetComponent<ComponentAnimator>();
			if (animator != nullptr) {
				animator->OnPlay();
			}
		}

		if (!App->objects->current_scripts.empty() && Time::IsInGameState()) {
			Prefab::InitScripts(obj);
		}

		// Navigation
		auto ui = parent->GetComponentsInChildrenRecursive<ComponentUI>();
		auto uiParent = parent->GetComponents<ComponentUI>();
		ui.insert(ui.end(), uiParent.begin(), uiParent.end());
		for each (ComponentUI* uiElement in ui) {
			uiElement->ReSetIDNavigation();
		}

		App->objects->ReAttachUIScriptEvents();
		obj->ResetIDs();

		for each (ComponentUI * uiElement in ui) {
			uiElement->ReSetIDNavigation();
		}

		obj->SetPrefab(ID);
		obj->transform->SetLocalPosition(pos);
		if (set_selected) {
			App->objects->SetNewSelectedObject(obj, false);
			/*App->camera->fake_camera->Look(parent->children.back()->GetBB().CenterPoint());
			App->camera->reference = parent->children.back()->GetBB().CenterPoint();*/
		}

		ComponentRigidBody* rb = (ComponentRigidBody*)(obj)->GetComponent(ComponentType::RIGID_BODY);
		if (rb)
			rb->SetPosition(pos);

		// TODO: check this
		/*ComponentCharacterController* character_controller = (ComponentCharacterController*)(obj)->GetComponent(ComponentType::CHARACTER_CONTROLLER);
		if (character_controller)
			character_controller->SetPosition(pos);*/

		if (check_childrens)
		{
			for (auto it_child = obj->children.begin(); it_child != obj->children.end(); ++it_child)
			{
				CheckChildren(*it_child, float3::zero());
			}
		}
		
		delete prefab;
		App->objects->inPrefabCreation = false;
		return obj;
	}
	else {
		LOG_ENGINE("Error loading prefab %s", path.data());
	}
	App->objects->inPrefabCreation = false;
	return nullptr;
}

void ResourcePrefab::CheckChildren(GameObject* game_object, float3 pos)
{
	ComponentRigidBody* rb = (ComponentRigidBody*)(game_object)->GetComponent(ComponentType::RIGID_BODY);
	if (rb)
	{
		rb->SetPosition(pos);
		for (auto it_child = game_object->children.begin(); it_child != game_object->children.end(); ++it_child)
		{
			CheckChildren(*it_child, rb->GetPosition());
		}
		return;
	}

	// TODO: check this
	/*ComponentCharacterController* character_controller = (ComponentCharacterController*)(game_object)->GetComponent(ComponentType::CHARACTER_CONTROLLER);
	if (character_controller)
	{
		character_controller->SetPosition(pos);
		for (auto it_child = game_object->children.begin(); it_child != game_object->children.end(); ++it_child)
		{
			CheckChildren(*it_child, character_controller->GetPosition());
		}
		return;
	}*/

	for (auto it_child = game_object->children.begin(); it_child != game_object->children.end(); ++it_child)
	{
		CheckChildren(*it_child, pos);
	}
}
