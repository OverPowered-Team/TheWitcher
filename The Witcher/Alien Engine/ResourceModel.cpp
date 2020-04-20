#include "ResourceModel.h"
#include "Globals.h"
#include "ResourceMesh.h"
#include "ResourceAnimation.h"
#include "ResourceBone.h"
#include "ModuleFileSystem.h"
#include "ComponentDeformableMesh.h"
#include "Application.h"
#include <algorithm>
#include "PanelProject.h"
#include "ModuleUI.h"
#include "ReturnZ.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"
#include "ModuleResources.h"
#include "ModuleCamera3D.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentBone.h"
#include "mmgr/mmgr.h"

ResourceModel::ResourceModel() : Resource()
{
	type = ResourceType::RESOURCE_MODEL;
}

ResourceModel::~ResourceModel()
{
	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) { // aixo passa pq es fa el delete del modul resources i aquest respurce resource_mesh ja la borrat alla
			delete* item;
			*item = nullptr;
		}
	}
	meshes_attached.clear();
}

bool ResourceModel::CreateMetaData(const u64& force_id)
{	
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;
	meta_data_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
	std::string* meta_mesh_paths = nullptr;
	std::string* meta_animation_paths = nullptr;
	std::string* meta_bones_paths = nullptr;
	std::string* meta_materials_paths = nullptr;
	int num_anims = 0;
	if (force_id != 0) 
	{
		JSON_Value* value = json_parse_file(meta_data_path.data());
		JSON_Object* object = json_value_get_object(value);

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* meta = new JSONfilepack(meta_data_path.data(), object, value);

			meta_mesh_paths = meta->GetArrayString("Meta.PathMeshes");
			meta_animation_paths = meta->GetArrayString("Meta.PathAnimations");
			meta_bones_paths = meta->GetArrayString("Meta.PathBones");
			meta_materials_paths = meta->GetArrayString("Meta.PathMaterials");
			num_anims = meta->GetNumber("Meta.NumAnimations");
			delete meta;
		}
	}

	std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, alien_path.data());

	if (alien_value != nullptr && alien_object != nullptr) 
	{
		JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID).data());

		std::string library_path = std::string(LIBRARY_MODELS_FOLDER) + std::string(std::to_string(ID) + ".alienModel");

		alien->SetNumber("Meta.NumMeshes", meshes_attached.size());

		std::string* meshes_paths = new std::string[meshes_attached.size()];
		std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
		for (; item != meshes_attached.end(); ++item) {
			if ((*item) != nullptr) 
			{
				if (meta_mesh_paths != nullptr) {
					std::string path_ = App->file_system->GetBaseFileName(meta_mesh_paths[item - meshes_attached.begin()].data()); //std::stoull().data());
					(*item)->CreateMetaData(std::stoull(path_));
				}
				else {
					(*item)->CreateMetaData();
				}

				meshes_paths[item - meshes_attached.begin()] = (*item)->GetLibraryPath();
			}
		}
		alien->SetString("Meta.Name", name.data());
		alien->SetNumber("Meta.NumBones", bones_attached.size());

		std::string* bones_paths = new std::string[bones_attached.size()];
		for (int i = 0; i < bones_attached.size(); ++i)
		{
			if (meta_bones_paths != nullptr)
			{
				std::string path_ = App->file_system->GetBaseFileName(meta_bones_paths[i].data()); //std::stoull().data());
				bones_attached[i]->CreateMetaData(std::stoull(path_));
			}
			else
			{
				bones_attached[i]->CreateMetaData();
			}

			bones_paths[i] = bones_attached[i]->GetLibraryPath();
			LOG_ENGINE("Created alienBone file %s", bones_attached[i]->GetLibraryPath());
		}

		alien->SetNumber("Meta.NumAnimations", animations_attached.size()); 
		JSONArraypack* anim_array_alien = alien->InitNewArray("Meta.Animations");

		std::string* animation_paths = new std::string[animations_attached.size()];
		for (int i = 0; i < animations_attached.size(); ++i)
		{
			if (meta_animation_paths != nullptr && i < num_anims)
			{
				std::string path_ = App->file_system->GetBaseFileName(meta_animation_paths[i].data()); //std::stoull().data());
				animations_attached[i]->CreateMetaData(std::stoull(path_));
			}
			else 
			{
				animations_attached[i]->CreateMetaData();
			}

			anim_array_alien->SetAnotherNode();
			anim_array_alien->SetString("Name", animations_attached[i]->name.data());
			anim_array_alien->SetBoolean("Loops", animations_attached[i]->loops);
			anim_array_alien->SetNumber("Start_Tick", animations_attached[i]->start_tick);
			anim_array_alien->SetNumber("End_Tick", animations_attached[i]->end_tick);

			animation_paths[i] = animations_attached[i]->GetLibraryPath();
			LOG_ENGINE("Created alienAnimation file %s", animations_attached[i]->GetLibraryPath());
		}

		alien->SetNumber("Meta.NumMaterials", materials_attached.size());

		std::string* materials_path = new std::string[materials_attached.size()];
		for (int i = 0; i < materials_attached.size(); ++i)
		{
			if (meta_materials_paths == nullptr) {
				std::string materialName = materials_attached[i]->GetName();
				App->ui->panel_project->GetUniqueFileName(materialName, MATERIALS_FOLDER);

				materials_attached[i]->SetName(materialName.c_str());
				materials_attached[i]->SetAssetsPath(std::string(MATERIALS_FOLDER + materialName + ".material").data());
				materials_attached[i]->SaveResource();
			}

			materials_path[i] = materials_attached[i]->GetLibraryPath();
		}

		alien->SetArrayString("Meta.PathMeshes", meshes_paths, meshes_attached.size());
		alien->SetArrayString("Meta.PathAnimations", animation_paths, animations_attached.size());
		alien->SetArrayString("Meta.PathBones", bones_paths, bones_attached.size());
		alien->SetArrayString("Meta.PathMaterials", materials_path, materials_attached.size());

		JSONArraypack* nodes = alien->InitNewArray("Nodes");
		for (uint i = 0; i < model_nodes.size(); ++i) {
			nodes->SetAnotherNode();
			nodes->SetFloat3("pos", model_nodes[i].pos);
			nodes->SetFloat3("scale", model_nodes[i].scale);
			nodes->SetQuat("rot", model_nodes[i].rot);
			nodes->SetNumber("nodeNum", model_nodes[i].node_num);
			nodes->SetNumber("parentNum", model_nodes[i].parent_num);
			nodes->SetString("nodeName", model_nodes[i].name.data());
			nodes->SetString("parentName", model_nodes[i].parent_name.data());
			nodes->SetNumber("meshIndex", model_nodes[i].mesh);
			nodes->SetNumber("materialIndex", model_nodes[i].material);
			nodes->SetNumber("numBones", model_nodes[i].bones.size());
			JSONArraypack* bones = nodes->InitNewArray("bonesIndex");
			for (uint j = 0; j < model_nodes[i].bones.size(); ++j)
			{
				bones->SetAnotherNode();
				bones->SetNumber(std::to_string(j).data(), model_nodes[i].bones[j]);
			}
		}

		if (meta_mesh_paths != nullptr)
			delete[] meta_mesh_paths;
		delete[] meshes_paths;

		if (meta_animation_paths != nullptr)
			delete[] meta_animation_paths;
		delete[] animation_paths;

		if (meta_materials_paths != nullptr)
			delete[] meta_materials_paths;
		delete[] materials_path;

		if (meta_bones_paths != nullptr)
			delete[] meta_bones_paths;
		delete[] bones_paths;
			
		alien->FinishSave();
		delete alien;

		App->file_system->Copy(alien_path.data(), library_path.data());
		meta_data_path = library_path.data();

		return true;
	}
	else {
		LOG_ENGINE("Error creating meta with path %s", meta_data_path.data());
		return false;
	}
}

bool ResourceModel::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	path = std::string(assets_file_path);
	meta_data_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_data_path.data(), object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));
		int num_meshes = meta->GetNumber("Meta.NumMeshes");
		int num_anims = meta->GetNumber("Meta.NumAnimations");
		int num_bones = meta->GetNumber("Meta.NumBones");
		int num_materials = meta->GetNumber("Meta.NumMaterials");
		std::string* mesh_paths = meta->GetArrayString("Meta.PathMeshes");
		std::string* anim_paths = meta->GetArrayString("Meta.PathAnimations");
		std::string* bones_paths = meta->GetArrayString("Meta.PathBones");
		std::string* materials_path = meta->GetArrayString("Meta.PathMaterials");

		for (uint i = 0; i < num_materials; ++i) {
			u64 matID = std::stoull(App->file_system->GetBaseFileName(materials_path[i].data()));
			ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResourceWithID(matID);
			if (mat != nullptr) {
				materials_attached.push_back(mat);
			}
		}
		for (uint i = 0; i < num_meshes; ++i) {
			if (!App->file_system->Exists(mesh_paths[i].data())) {
				delete[] mesh_paths;
				delete meta;
				return false;
			}
		}
		for (uint i = 0; i < num_anims; ++i) {
			if (!App->file_system->Exists(anim_paths[i].data())) {
				delete[] anim_paths;
				delete meta;
				return false;
			}
		}
		for (uint i = 0; i < num_bones; ++i) {
			if (!App->file_system->Exists(bones_paths[i].data())) {
				delete[] bones_paths;
				delete meta;
				return false;
			}
		}

		JSONArraypack* nodes = meta->GetArray("Nodes");
		nodes->GetFirstNode();
		for (uint i = 0; i < nodes->GetArraySize(); ++i) {
			ModelNode node;
			node.pos = nodes->GetFloat3("pos");
			node.scale = nodes->GetFloat3("scale");
			node.rot = nodes->GetQuat("rot");
			node.node_num = nodes->GetNumber("nodeNum");
			node.parent_num = nodes->GetNumber("parentNum");
			node.name = nodes->GetString("nodeName");
			node.parent_name = nodes->GetString("parentName");
			node.mesh = nodes->GetNumber("meshIndex");
			node.material = nodes->GetNumber("materialIndex");
			uint num_bones = nodes->GetNumber("numBones");
			JSONArraypack* bones = nodes->GetArray("bonesIndex");
			for (uint j = 0; j < num_bones; ++j)
			{
				node.bones.push_back(bones->GetNumber(std::to_string(j).data()));
				bones->GetAnotherNode();
			}
			model_nodes.push_back(node);
			nodes->GetAnotherNode();
		}

		delete[] anim_paths;
		delete[] mesh_paths;
		delete[] materials_path;
		delete[] bones_paths;
		delete meta;

		// InitMeshes
		std::string library_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";
		meta_data_path = library_path;
		JSON_Value* mesh_value = json_parse_file(library_path.data());
		JSON_Object* mesh_object = json_value_get_object(mesh_value);

		if (mesh_value != nullptr && mesh_object != nullptr) {

			JSONfilepack* model = new JSONfilepack(library_path.data(), mesh_object, mesh_value);

			name = model->GetString("Meta.Name");

			std::string* mesh_path = model->GetArrayString("Meta.PathMeshes");
			std::string* anim_path = model->GetArrayString("Meta.PathAnimations");
			std::string* materials_path = model->GetArrayString("Meta.PathMaterials");
			std::string* bones_path = model->GetArrayString("Meta.PathBones");

			for (uint i = 0; i < num_meshes; ++i) {
				ResourceMesh* r_mesh = new ResourceMesh();
				if (r_mesh->ReadBaseInfo(mesh_path[i].data())) {
					meshes_attached.push_back(r_mesh);
				}
				else {
					LOG_ENGINE("Error loading %s", mesh_path[i].data());
					delete r_mesh;
				}
			}
			for (uint i = 0; i < num_anims; ++i) {
				ResourceAnimation* r_anim = new ResourceAnimation();
				if (r_anim->ReadBaseInfo(anim_path[i].data())) {
					animations_attached.push_back(r_anim);
				}
				else {
					LOG_ENGINE("Error loading %s", anim_path[i].data());
					delete r_anim;
				}
			}
			for (uint i = 0; i < num_bones; ++i) {
				ResourceBone* r_bone = new ResourceBone();
				if (r_bone->ReadBaseInfo(bones_path[i].data())) {
					bones_attached.push_back(r_bone);
				}
				else {
					LOG_ENGINE("Error loading %s", bones_path[i].data());
					delete r_bone;
				}
			}

			delete[] anim_path;
			delete[] mesh_path;
			delete[] materials_path;
			delete[] bones_path;
			delete model;
			App->resources->AddResource(this);
		}
	}
	else {
		ret = false;
	}
	return ret;
}

void ResourceModel::ReadLibrary(const char* meta_data)
{
	meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	JSON_Value* mesh_value = json_parse_file(meta_data_path.data());
	JSON_Object* mesh_object = json_value_get_object(mesh_value);

	if (mesh_value != nullptr && mesh_object != nullptr) {

		JSONfilepack* model = new JSONfilepack(meta_data_path.data(), mesh_object, mesh_value);

		name = model->GetString("Meta.Name");
		int num_meshes = model->GetNumber("Meta.NumMeshes");
		std::string* mesh_path = model->GetArrayString("Meta.PathMeshes");
		int num_anims = model->GetNumber("Meta.NumAnimations");
		std::string* anim_path = model->GetArrayString("Meta.PathAnimations");
		int num_bones = model->GetNumber("Meta.NumBones");
		std::string* bones_path = model->GetArrayString("Meta.PathBones");
		int num_materials = model->GetNumber("Meta.NumMaterials");
		std::string* materials_path = model->GetArrayString("Meta.PathMaterials");

		for (uint i = 0; i < num_materials; ++i) {
			u64 matID = std::stoull(App->file_system->GetBaseFileName(materials_path[i].data()));
			ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResourceWithID(matID);
			if (mat != nullptr) {
				materials_attached.push_back(mat);
			}
		}

		for (uint i = 0; i < num_meshes; ++i) {
			ResourceMesh* r_mesh = new ResourceMesh();
			if (r_mesh->ReadBaseInfo(mesh_path[i].data())) {
				meshes_attached.push_back(r_mesh);
			}
			else {
				LOG_ENGINE("Error loading %s", mesh_path[i].data());
				delete r_mesh;
			}
		}
		for (uint i = 0; i < num_anims; ++i) {
			ResourceAnimation* r_anim = new ResourceAnimation();
			if (r_anim->ReadBaseInfo(anim_path[i].data())) {
				animations_attached.push_back(r_anim);
			}
			else {
				LOG_ENGINE("Error loading %s", anim_path[i].data());
				delete r_anim;
			}
		}
		for (uint i = 0; i < num_bones; ++i) {
			ResourceBone* r_bone = new ResourceBone();
			if (r_bone->ReadBaseInfo(bones_path[i].data())) {
				bones_attached.push_back(r_bone);
			}
			else {
				LOG_ENGINE("Error loading %s", bones_path[i].data());
				delete r_bone;
			}
		}

		JSONArraypack* nodes = model->GetArray("Nodes");
		nodes->GetFirstNode();
		for (uint i = 0; i < nodes->GetArraySize(); ++i) {
			ModelNode node;
			node.pos = nodes->GetFloat3("pos");
			node.scale = nodes->GetFloat3("scale");
			node.rot = nodes->GetQuat("rot");
			node.node_num = nodes->GetNumber("nodeNum");
			node.parent_num = nodes->GetNumber("parentNum");
			node.name = nodes->GetString("nodeName");
			node.parent_name = nodes->GetString("parentName");
			node.mesh = nodes->GetNumber("meshIndex");
			node.material = nodes->GetNumber("materialIndex");
			uint num_bones = nodes->GetNumber("numBones");
			JSONArraypack* bones = nodes->GetArray("bonesIndex");
			for (uint j = 0; j < num_bones; ++j)
			{
				node.bones.push_back(bones->GetNumber(std::to_string(j).data()));
				bones->GetAnotherNode();
			}
			model_nodes.push_back(node);
			nodes->GetAnotherNode();
		}

		delete[] mesh_path;
		delete[] anim_path;
		delete[] bones_path;
		delete[] materials_path;
		delete model;
		App->resources->AddResource(this);
	}
}

bool ResourceModel::LoadMemory()
{
	bool ret = true;

	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) {
			(*item)->LoadMemory();
		}
	}

	std::vector<ResourceAnimation*>::iterator anim_item = animations_attached.begin();
	for (; anim_item != animations_attached.end(); ++anim_item) {
		if (*anim_item != nullptr) {
			(*anim_item)->LoadMemory();
		}
	}

	std::vector<ResourceBone*>::iterator bone_item = bones_attached.begin();
	for (; bone_item != bones_attached.end(); ++bone_item) {
		if (*bone_item != nullptr) {
			(*bone_item)->LoadMemory();
		}
	}

	return ret;
}

void ResourceModel::FreeMemory()
{
	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) {
			(*item)->FreeMemory();
		}
	}

	std::vector<ResourceAnimation*>::iterator anim_item = animations_attached.begin();
	for (; anim_item != animations_attached.end(); ++anim_item) {
		if (*anim_item != nullptr) {
			(*anim_item)->FreeMemory();
		}
	}

	std::vector<ResourceBone*>::iterator bone_item = bones_attached.begin();
	for (; bone_item != bones_attached.end(); ++bone_item) {
		if (*bone_item != nullptr) {
			(*bone_item)->FreeMemory();
		}
	}
}

bool ResourceModel::DeleteMetaData()
{
	remove(std::string(LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel").data());

	std::vector<ResourceMesh*>::iterator item = meshes_attached.begin();
	for (; item != meshes_attached.end(); ++item) {
		if (*item != nullptr) {
			(*item)->DeleteMetaData();
		}
	}
	meshes_attached.clear();

	std::vector<ResourceAnimation*>::iterator item_anim = animations_attached.begin();
	for (; item_anim != animations_attached.end(); ++item_anim)
	{
		if ((*item_anim) != nullptr)
		{
			(*item_anim)->DeleteMetaData();
		}
	}
	animations_attached.clear();

	std::vector<ResourceBone*>::iterator item_bone = bones_attached.begin();
	for (; item_bone != bones_attached.end(); ++item_bone)
	{
		if ((*item_bone) != nullptr)
		{
			(*item_bone)->DeleteMetaData();
		}
	}
	bones_attached.clear();

	std::vector<ResourceMaterial*>::iterator item_mat = materials_attached.begin();
	for (; item_mat != materials_attached.end(); ++item_mat)
	{
		if ((*item_mat) != nullptr)
		{
			(*item_mat)->DeleteMetaData();
		}
	}
	materials_attached.clear();

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end()) 
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceModel::UpdateAnimationInfo()
{
	JSON_Value* mesh_value = json_parse_file(meta_data_path.data());
	JSON_Object* mesh_object = json_value_get_object(mesh_value);

	if (mesh_value != nullptr && mesh_object != nullptr) {

		JSONfilepack* model = new JSONfilepack(meta_data_path.data(), mesh_object, mesh_value);

		name = model->GetString("Meta.Name");
		int num_meshes = model->GetNumber("Meta.NumMeshes");
		std::string* mesh_path = model->GetArrayString("Meta.PathMeshes");
		int num_anims = model->GetNumber("Meta.NumAnimations");
		std::string* anim_path = model->GetArrayString("Meta.PathAnimations");
		int num_bones = model->GetNumber("Meta.NumBones");
		std::string* bones_path = model->GetArrayString("Meta.PathBones");
		int num_materials = model->GetNumber("Meta.NumMaterials");
		std::string* materials_path = model->GetArrayString("Meta.PathMaterials");

		std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

		JSON_Value* alien_value = json_value_init_object();
		JSON_Object* alien_object = json_value_get_object(alien_value);
		json_serialize_to_file_pretty(alien_value, alien_path.data());
		JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID).data());

		std::string library_path = std::string(LIBRARY_MODELS_FOLDER) + std::string(std::to_string(ID) + ".alienModel");

		alien->SetNumber("Meta.NumMeshes", meshes_attached.size());
		alien->SetString("Meta.Name", name.data());
		alien->SetNumber("Meta.NumBones", bones_attached.size());
		alien->SetNumber("Meta.NumAnimations", animations_attached.size());
		alien->SetNumber("Meta.NumMaterials", materials_attached.size());


		JSONArraypack* anim_array_alien = alien->InitNewArray("Meta.Animations");

		std::string* animation_paths = new std::string[animations_attached.size()];
		for (int i = 0; i < animations_attached.size(); ++i)
		{
			if (anim_path != nullptr && i < num_anims)
			{
				std::string path_ = App->file_system->GetBaseFileName(anim_path[i].data()); //std::stoull().data());
				animations_attached[i]->CreateMetaData(std::stoull(path_));
			}
			else
			{
				animations_attached[i]->CreateMetaData();
			}

			anim_array_alien->SetAnotherNode();
			anim_array_alien->SetString("Name", animations_attached[i]->name.data());
			anim_array_alien->SetBoolean("Loops", animations_attached[i]->loops);
			anim_array_alien->SetNumber("Start_Tick", animations_attached[i]->start_tick);
			anim_array_alien->SetNumber("End_Tick", animations_attached[i]->end_tick);

			animation_paths[i] = animations_attached[i]->GetLibraryPath();
			LOG_ENGINE("Created alienAnimation file %s", animations_attached[i]->GetLibraryPath());
		}

		alien->SetArrayString("Meta.PathMeshes", mesh_path, meshes_attached.size());
		alien->SetArrayString("Meta.PathAnimations", animation_paths, animations_attached.size());
		alien->SetArrayString("Meta.PathBones", bones_path, bones_attached.size());
		alien->SetArrayString("Meta.PathMaterials", materials_path, materials_attached.size());

		JSONArraypack* nodes = alien->InitNewArray("Nodes");
		for (uint i = 0; i < model_nodes.size(); ++i) {
			nodes->SetAnotherNode();
			nodes->SetFloat3("pos", model_nodes[i].pos);
			nodes->SetFloat3("scale", model_nodes[i].scale);
			nodes->SetQuat("rot", model_nodes[i].rot);
			nodes->SetNumber("nodeNum", model_nodes[i].node_num);
			nodes->SetNumber("parentNum", model_nodes[i].parent_num);
			nodes->SetString("nodeName", model_nodes[i].name.data());
			nodes->SetString("parentName", model_nodes[i].parent_name.data());
			nodes->SetNumber("meshIndex", model_nodes[i].mesh);
			nodes->SetNumber("materialIndex", model_nodes[i].material);
			nodes->SetNumber("numBones", model_nodes[i].bones.size());
			JSONArraypack* bones = nodes->InitNewArray("bonesIndex");
			for (uint j = 0; j < model_nodes[i].bones.size(); ++j)
			{
				bones->SetAnotherNode();
				bones->SetNumber(std::to_string(j).data(), model_nodes[i].bones[j]);
			}
		}

		alien->FinishSave();
		delete alien;

		App->file_system->Copy(alien_path.data(), library_path.data());

		delete[] mesh_path;
		delete[] anim_path;
		delete[] bones_path;
		delete[] materials_path;
		delete model;
	}
}

void ResourceModel::ConvertToGameObjects()
{
	if (model_nodes.empty())
		return;

	App->objects->ignore_cntrlZ = true;

	std::sort(model_nodes.begin(), model_nodes.end(), ResourceModel::SortByFamilyNumber);

	GameObject* parent = nullptr;

	if (model_nodes.size() > 1) {
		parent = App->objects->CreateEmptyGameObject(nullptr, false);
		parent->SetName(name.data());
	}
	else {
		parent = App->objects->GetRoot(false);
	}

	std::vector<std::pair<uint, GameObject*>> objects_created;
	objects_created.push_back({ 1,parent });

	GameObject* root_bone = nullptr;
	for (uint i = 0; i < model_nodes.size(); ++i) {
		CreateGameObject(model_nodes[i], objects_created, root_bone);
	}
	App->objects->GetRoot(false)->children.back()->transform->RecalculateTransform();
	App->objects->ignore_cntrlZ = false;

	for each (std::pair<int,GameObject*> object in objects_created)
	{
		ComponentDeformableMesh* def_mesh = object.second->GetComponent<ComponentDeformableMesh>();
		if (def_mesh && root_bone)
		{
			def_mesh->AttachSkeleton(root_bone->transform);
		}
	}

	App->objects->SetNewSelectedObject(App->objects->GetRoot(false)->children.back(), false);
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, App->objects->GetRoot(false)->children.back());
	App->camera->fake_camera->Look(App->objects->GetRoot(false)->children.back()->GetBB().CenterPoint());
	App->camera->reference = App->objects->GetRoot(false)->children.back()->GetBB().CenterPoint();
}

bool ResourceModel::SortByFamilyNumber(const ModelNode& node1, const ModelNode& node2)
{
	return node1.node_num < node2.node_num;
}

GameObject* ResourceModel::CreateGameObject(const ModelNode& node, std::vector<std::pair<uint, GameObject*>>& objects_created, GameObject*& root_bone)
{
	GameObject* ret = nullptr;

	for (uint i = 0; i < objects_created.size(); ++i) {
		if (objects_created[i].first == node.parent_num && strcmp(objects_created[i].second->name, (model_nodes.size() > 1) ? node.parent_name.data() : App->objects->GetRoot(true)->GetName()) == 0) 
		{
			ret = new GameObject(objects_created[i].second, node.pos, node.rot, node.scale);
			ret->SetName(node.name.data());

			if (node.mesh >= 0) {
				ResourceMesh* mesh = meshes_attached[node.mesh];
				if (mesh != nullptr) {
					ComponentMesh* Cmesh = nullptr;
					if (mesh->deformable)
					{
						Cmesh = new ComponentDeformableMesh(ret);
					}
					else
						Cmesh = new ComponentMesh(ret);

					mesh->IncreaseReferences();
					Cmesh->SetResourceMesh(mesh);
					ret->AddComponent(Cmesh);
				}

				if (node.material >= 0)
				{
					if (node.material < materials_attached.size()) {
						ResourceMaterial* material = materials_attached[node.material];
						if (material != nullptr)
						{
							ComponentMaterial* Cmat = new ComponentMaterial(ret);
							Cmat->SetMaterial(material);
							ret->AddComponent(Cmat);
						}
					}
				}

			}
			if (node.bones.size() > 0)
			{
				if (!root_bone)
					root_bone = ret;

				for (uint i = 0; i < node.bones.size(); ++i)
				{
					ResourceBone* bone = bones_attached[node.bones[i]];
					if (bone != nullptr) {
						ComponentBone* c_bone = new ComponentBone(ret);
						bone->IncreaseReferences();
						c_bone->bone = bone;
						ret->AddComponent(c_bone);
					}
				}
			}
			objects_created.push_back({ node.node_num, ret });
			break;
		}
	}

	return ret;
}


