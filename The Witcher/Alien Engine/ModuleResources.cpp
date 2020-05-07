#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "SDL/include/SDL_assert.h"
#include "Resource_.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ResourceFont.h"
#include "ModuleImporter.h"
#include "Application.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ModuleAudio.h"
#include "ResourceAnimatorController.h"
#include "RandomHelper.h"
#include "ResourceScene.h"
#include "PanelProject.h"
#include "ResourcePrefab.h"
#include "ResourceAudio.h"
#include "ModuleCamera3D.h"
#include "ResourceMaterial.h"
#include "ComponentCamera.h"
#include "FileNode.h"
#include "ResourceScript.h"
#include "Event.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"
#include "ModuleUI.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
	name = "Resources";
}

ModuleResources::~ModuleResources()
{

	std::vector<Resource*>::iterator item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr) {
			if ((*item)->GetType() == ResourceType::RESOURCE_MODEL)
				static_cast<ResourceModel*>(*item)->meshes_attached.clear();

			delete* item;
			*item = nullptr;
		}
	}

	resources.clear();

	delete cube;
	delete sphere;
	delete rock;
	delete torus;
	delete dodecahedron;
	delete icosahedron;
	delete octahedron;

	delete default_material;

	delete light_mesh;
	delete camera_mesh;
}

bool ModuleResources::Start()
{
	OPTICK_EVENT();
#ifndef GAME_VERSION
	// Load Icons
	icons.jpg_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_jpg.png");
	icons.png_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_png.png");
	icons.dds_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_dds.png");
	icons.tga_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_tga.png");
	icons.shader_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_shader.png");
	icons.folder = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_folder.png");
	icons.script_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_script.png");
	icons.prefab_icon = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_prefab.png");
	icons.model = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_model.png");
	icons.return_icon = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_return.png");
	icons.scene_file = App->importer->LoadEngineTexture("Configuration/EngineTextures/icon_scene.png");
	icons.move_transform = App->importer->LoadEngineTexture("Configuration/EngineTextures/move.png");
	icons.rotate_transform = App->importer->LoadEngineTexture("Configuration/EngineTextures/rotate.png");
	icons.scale_transform = App->importer->LoadEngineTexture("Configuration/EngineTextures/scales.png");
	icons.redo = App->importer->LoadEngineTexture("Configuration/EngineTextures/redo.png");
	icons.undo = App->importer->LoadEngineTexture("Configuration/EngineTextures/undo.png");
	icons.global = App->importer->LoadEngineTexture("Configuration/EngineTextures/global.png");
	icons.local = App->importer->LoadEngineTexture("Configuration/EngineTextures/local.png");
	icons.play = App->importer->LoadEngineTexture("Configuration/EngineTextures/play.png");
	icons.pause = App->importer->LoadEngineTexture("Configuration/EngineTextures/pause.png");
	icons.once = App->importer->LoadEngineTexture("Configuration/EngineTextures/once.png");
	icons.box = App->importer->LoadEngineTexture("Configuration/EngineTextures/box.png");
	icons.prefab = App->importer->LoadEngineTexture("Configuration/EngineTextures/prefab.png");
	icons.prefab_lock = App->importer->LoadEngineTexture("Configuration/EngineTextures/prefab_lock.png");

	camera_mesh = App->importer->LoadEngineModels("Configuration/Engine Models/camera.FBX");
	light_mesh = App->importer->LoadEngineModels("Configuration/Engine Models/bulb.fbx");

	assets = new FileNode();
	assets->is_file = false;
	assets->is_base_file = true;
	assets->name = "Assets";

	App->file_system->DiscoverEverythig(assets);

#endif

	// Load Primitives as resource
	cube = new ResourceMesh();
	sphere = new ResourceMesh();
	rock = new ResourceMesh();
	torus = new ResourceMesh();
	dodecahedron = new ResourceMesh();
	icosahedron = new ResourceMesh();
	octahedron = new ResourceMesh();

	// TODO: look if all meta data has its fbx or texture if not remove meta data

	ReadAllMetaData();

	default_font = (ResourceFont*)GetResourceWithID(6090935666492539845);
	default_material = new ResourceMaterial();
	default_material->SetName("Default Material");
	//default_material = (ResourceMaterial*)GetResourceWithID(13753584922284142239);

#ifndef GAME_VERSION
	App->camera->fake_camera = new ComponentCamera(nullptr);
	App->camera->fake_camera->frustum.farPlaneDistance = 1000.0F;
	App->renderer3D->scene_fake_camera = App->camera->fake_camera;
#endif

	return true;
}

update_status ModuleResources::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	OPTICK_EVENT();

	return true;
}

void ModuleResources::AddResource(Resource* resource)
{
	if (resource != nullptr) {
		if (std::find(resources.begin(), resources.end(), resource) == resources.end())
			resources.push_back(resource);
	}
}

void ModuleResources::RemoveResource(Resource* resource)
{
	if (resource != nullptr) {
		std::vector<Resource*>::iterator it = std::find(resources.begin(), resources.end(), resource);
		if (it != resources.end())
			resources.erase(it);
	}
}

bool ModuleResources::CreateNewModelInstanceOf(const char* path)
{
	bool ret = false;
	std::vector<Resource*>::iterator item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_MODEL) {
			if (App->StringCmp((*item)->GetLibraryPath(), path)) {
				static_cast<ResourceModel*>(*item)->ConvertToGameObjects();
				ret = true;
				break;
			}
		}
	}
	return ret;
}

u64 ModuleResources::GetIDFromAlienPath(const char* path)
{
	u64 ID = 0;
	OPTICK_EVENT();
	JSON_Value* value = json_parse_file(path);
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(path, object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;
	}

	return ID;
}

Resource* ModuleResources::GetResourceWithID(const u64& ID)
{
	OPTICK_EVENT();

	if (ID == 0)
		return nullptr; 

	std::vector<Resource*>::iterator item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr && (*item)->GetID() == ID)
			return (*item);
	}
	LOG_ENGINE("No resource found with ID %i", ID);
	return nullptr;
}

const Resource* ModuleResources::GetResourceWithID(const u64& ID) const
{
	OPTICK_EVENT();
	std::vector<Resource*>::const_iterator item = resources.cbegin();
	for (; item != resources.cend(); ++item) {
		if (*item != nullptr && (*item)->GetID() == ID)
			return (*item);
	}
	LOG_ENGINE("No resource found with ID %i", ID);
	return nullptr;
}

std::vector<Resource*> ModuleResources::GetResourcesWithType(ResourceType type)
{
	OPTICK_EVENT();
	std::vector<Resource*> ret;
	for each (Resource* r in resources)
	{
		if (r->GetType() == type)
			ret.push_back(r);
	}

	return ret;
}

void ModuleResources::AddNewFileNode(const std::string& path, bool is_file)
{
	std::string folder = App->file_system->GetCurrentHolePathFolder(path);

	FileNode* parent = GetFileNodeByPath(folder, assets);

	std::string name_file = App->file_system->GetBaseFileNameWithExtension(path.data());

	bool exists = false;
	for (uint i = 0; i < parent->children.size(); ++i) {
		if (parent->children[i] != nullptr && App->StringCmp(parent->children[i]->name.data(), name_file.data())) {
			exists = true;
			break;
		}
	}
	if (!exists)
		parent->children.push_back(new FileNode(name_file, is_file, parent));

	if (App->ui->panel_project != nullptr)
		App->ui->panel_project->current_active_folder = parent;
}

u64 ModuleResources::GetRandomID()
{
	return Random::GetRandomID();
}

ResourceTexture * ModuleResources::GetTextureByName(const char * name)
{
	OPTICK_EVENT();
	ResourceTexture* ret = nullptr;

	std::vector<Resource*>::iterator item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && App->StringCmp(App->file_system->GetBaseFileName((*item)->GetAssetsPath()).data(),App->file_system->GetBaseFileName(name).data())) {
			return static_cast<ResourceTexture*>(*item);
		}
	}

	return ret;
}

const ResourceTexture* ModuleResources::GetTextureByName(const char* name) const
{
	OPTICK_EVENT();
	ResourceTexture* ret = nullptr;

	std::vector<Resource*>::const_iterator item = resources.cbegin();
	for (; item != resources.cend(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && App->StringCmp(App->file_system->GetBaseFileName((*item)->GetAssetsPath()).data(), App->file_system->GetBaseFileName(name).data())) {
			return static_cast<ResourceTexture*>(*item);
		}
	}

	return ret;
}

ResourceMesh* ModuleResources::GetPrimitive(const PrimitiveType& type)
{	
	ResourceMesh* ret = nullptr;

	switch (type)
	{
	case PrimitiveType::CUBE:
		ret = cube;
		break;
	case PrimitiveType::SPHERE_ALIEN:
		ret = sphere;
		break;
	case PrimitiveType::ROCK:
		ret = rock;
		break;
	case PrimitiveType::DODECAHEDRON:
		ret = dodecahedron;
		break;
	case PrimitiveType::OCTAHEDRON:
		ret = octahedron;
		break;
	case PrimitiveType::TORUS:
		ret = torus;
		break;
	case PrimitiveType::ICOSAHEDRON:
		ret = icosahedron;
		break;
	default:
		break;
	}
	
	if (ret != nullptr) {
		if (ret->NeedToLoad())
			CreatePrimitive(type, &ret);
		if (App->objects->enable_instancies) {
			++ret->references;
		}
	}
	return ret;
}

const ResourceMesh* ModuleResources::GetPrimitive(const PrimitiveType& type) const
{
	ResourceMesh* ret = nullptr;

	switch (type)
	{
	case PrimitiveType::CUBE:
		ret = cube;
		break;
	case PrimitiveType::SPHERE_ALIEN:
		ret = sphere;
		break;
	case PrimitiveType::ROCK:
		ret = rock;
		break;
	case PrimitiveType::DODECAHEDRON:
		ret = dodecahedron;
		break;
	case PrimitiveType::OCTAHEDRON:
		ret = octahedron;
		break;
	case PrimitiveType::TORUS:
		ret = torus;
		break;
	case PrimitiveType::ICOSAHEDRON:
		ret = icosahedron;
		break;
	default:
		break;
	}

	if (ret != nullptr) {
		if (ret->NeedToLoad()) {
			CreatePrimitive(type, &ret);
		}
		if (App->objects->enable_instancies) {
			++ret->references;
		}
	}
	return ret;
}

bool ModuleResources::Exists(const char * path, Resource** resource) const
{
	bool exists = false;

	std::vector<Resource*>::const_iterator item = resources.cbegin();
	for (; item != resources.cend(); ++item) {
		if (*item != nullptr && App->StringCmp(path, (*item)->GetAssetsPath())) {
			exists = true;
			if (resource != nullptr)
				*resource = (*item);
			break;
		}
	}

	return exists;
}

void ModuleResources::CreatePrimitive(const PrimitiveType& type, ResourceMesh** ret)
{
	(*ret)->is_primitive = true;
	par_shapes_mesh* par_mesh = nullptr;
	
	switch (type)
	{
	case PrimitiveType::CUBE: {
		par_mesh = par_shapes_create_cube();
		(*ret)->SetName("Cube");
		for (uint i = 0; i < par_mesh->npoints; i++)
		{
			par_mesh->points[i*3] -= 0.5;
			par_mesh->points[i*3+1] -= 0.5;
			par_mesh->points[i*3+2] -= 0.5;
		}
		break; }
	case PrimitiveType::SPHERE_ALIEN: {
		par_mesh = par_shapes_create_subdivided_sphere(1);
		(*ret)->SetName("Sphere");
		break; }
	case PrimitiveType::ROCK: {
		par_mesh = par_shapes_create_rock(3, 3);
		(*ret)->SetName("Rock");
		break; }
	case PrimitiveType::DODECAHEDRON: {
		par_mesh = par_shapes_create_dodecahedron();
		(*ret)->SetName("Dodecahedron");
		break; }
	case PrimitiveType::OCTAHEDRON: {
		par_mesh = par_shapes_create_octahedron();
		(*ret)->SetName("Octahedron");
		break; }
	case PrimitiveType::TORUS: {
		par_mesh = par_shapes_create_torus(12, 12, 0.5F);
		(*ret)->SetName("Torus");
		break; }
	case PrimitiveType::ICOSAHEDRON: {
		par_mesh = par_shapes_create_icosahedron();
		(*ret)->SetName("Icosahedron");
		break; }
	default: {
		break; }
	}
	
	App->importer->LoadParShapesMesh(par_mesh, *ret);
}

void ModuleResources::CreatePrimitive(const PrimitiveType& type, ResourceMesh** ret) const
{
	(*ret)->is_primitive = true;
	par_shapes_mesh* par_mesh = nullptr;

	switch (type)
	{
	case PrimitiveType::CUBE: {
		par_mesh = par_shapes_create_cube();
		(*ret)->SetName("Cube");
		break; }
	case PrimitiveType::SPHERE_ALIEN: {
		par_mesh = par_shapes_create_subdivided_sphere(5);
		(*ret)->SetName("Sphere");
		break; }
	case PrimitiveType::ROCK: {
		par_mesh = par_shapes_create_rock(3, 3);
		(*ret)->SetName("Rock");
		break; }
	case PrimitiveType::DODECAHEDRON: {
		par_mesh = par_shapes_create_dodecahedron();
		(*ret)->SetName("Dodecahedron");
		break; }
	case PrimitiveType::OCTAHEDRON: {
		par_mesh = par_shapes_create_octahedron();
		(*ret)->SetName("Octahedron");
		break; }
	case PrimitiveType::TORUS: {
		par_mesh = par_shapes_create_torus(12, 12, 0.5F);
		(*ret)->SetName("Torus");
		break; }
	case PrimitiveType::ICOSAHEDRON: {
		par_mesh = par_shapes_create_icosahedron();
		(*ret)->SetName("Icosahedron");
		break; }
	default: {
		break; }
	}

	App->importer->LoadParShapesMesh(par_mesh, *ret);
}

void ModuleResources::ReadHeaderFile(const char* path, std::vector<std::string>& current_scripts)
{
	ResourceScript* script = new ResourceScript();
	script->header_path = std::string(path);
	script->SetName(App->file_system->GetBaseFileName(path).data());
	for (auto item = current_scripts.begin(); item != current_scripts.end(); ++item) {
		if (App->StringCmp(App->file_system->GetBaseFileName(path).data(), App->file_system->GetBaseFileName((*item).data()).data())) {
			script->ReadBaseInfo((*item).data());
			current_scripts.erase(item);
			return;
		}
	}
	// if it goes here it is because it doesnt exist
	script->CreateMetaData();
}

void ModuleResources::ReloadScripts()
{
	OPTICK_EVENT();
	std::vector<std::string> files;
	std::vector<std::string> directories;

	App->file_system->DiscoverFiles(HEADER_SCRIPTS_FILE, files, directories, true);

	for (uint i = 0; i < files.size(); ++i) {
		if (files[i].back() == 'h') { // header file found
			bool exists = false;
			std::vector<Resource*>::iterator item = resources.begin();
			for (; item != resources.end(); ++item) {
				if ((*item) != nullptr && (*item)->GetType() == ResourceType::RESOURCE_SCRIPT) {
					ResourceScript* script = (ResourceScript*)*item;
					if (App->StringCmp(script->header_path.data(), files[i].data())) {
						if (script->NeedReload()) {
							script->Reimport();
						}
						exists = true;
						script->reload_completed = true;
						break;
					}
				}
			}
			if (!exists) {
				ResourceScript* script = new ResourceScript();
				script->header_path = std::string(files[i].data());
				script->SetName(App->file_system->GetBaseFileName(files[i].data()).data());
				script->CreateMetaData(); 
				script->reload_completed = true;
			}
		}
	}

	std::vector<Resource*>::iterator item = resources.begin();
	while (item != resources.end()) {
		if ((*item) != nullptr && (*item)->GetType() == ResourceType::RESOURCE_SCRIPT) {
			ResourceScript* script = (ResourceScript*)*item;
			if (script->reload_completed) {
				script->reload_completed = false;
			}
			else {
				remove((*item)->GetAssetsPath());
				delete* item;
				(*item) = nullptr;
				item = resources.erase(item);
				continue;
			}
		}
		++item;
	}

	App->ui->panel_project->RefreshAllNodes();
}

ResourceScene* ModuleResources::GetSceneByName(const char* name)
{
	auto item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_SCENE) {
			if (App->StringCmp((*item)->GetName(), name)) {
				return dynamic_cast<ResourceScene*>(*item);
			}
		}
	}
	return nullptr;
}

bool ModuleResources::GetShaders(std::vector<ResourceShader*>& to_fill)
{
	for (auto res = resources.begin(); res != resources.end(); res++) {
		if ((*res)->GetType() == ResourceType::RESOURCE_SHADER)
			to_fill.push_back((ResourceShader*)(*res));
	}

	return !to_fill.empty();
}

ResourceShader* ModuleResources::GetShaderByName(std::string shaderName)
{
	ResourceShader* desiredShader = nullptr; 

	for (auto res = resources.begin(); res != resources.end(); res++) {
		if ((*res)->GetType() == ResourceType::RESOURCE_SHADER)
		{
			if (App->StringCmp((*res)->GetName(), shaderName.c_str()))
			{
				desiredShader = (ResourceShader*)(*res);
				return desiredShader;
			}
		}
	}
	LOG_ENGINE("No shader found with name %s", name.c_str());
	return desiredShader;
}

ResourceMaterial* ModuleResources::GetMaterialByName(const char* name)
{
	ResourceMaterial* desiredMaterial = nullptr;

	for (auto res = resources.begin(); res != resources.end(); res++) {
		if ((*res)->GetType() == ResourceType::RESOURCE_MATERIAL)
		{
			if (App->StringCmp((*res)->GetName(), name))
			{
				desiredMaterial = (ResourceMaterial*)(*res);
				break;
			}
		}
	}
	LOG_ENGINE("No material found with name %s", name);
	return desiredMaterial;
}

const uint ModuleResources::GetTextureidByID(const u64& ID) const // This needs to be redifined
{
	OPTICK_EVENT();

	if (ID == 0)
		return -1;

	std::vector<Resource*>::const_iterator item = resources.cbegin();
	for (; item != resources.cend(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && (*item)->GetID() == ID) {
			return static_cast<ResourceTexture*>(*item)->id;
		}
	}

	LOG_ENGINE("No texture found with ID %i", ID);
	return -1;
}

ResourceTexture* ModuleResources::GetTextureByID(const u64& ID)
{
	OPTICK_EVENT();

	if (ID == 0)
		return nullptr;

	std::vector<Resource*>::const_iterator item = resources.cbegin();
	for (; item != resources.cend(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && (*item)->GetID() == ID) {
			return (ResourceTexture*)(*item);
		}
	}

	LOG_ENGINE("No texture found with ID %i", ID);
	return nullptr;
}

ResourceFont* ModuleResources::GetFontByName(const char* name)
{
	auto item = resources.begin();
	for (; item != resources.end(); ++item) {
		if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_FONT) {
			if (App->StringCmp((*item)->GetName(), name)) {
				return dynamic_cast<ResourceFont*>(*item);
			}
		}
	}
	return nullptr;
}

FileNode* ModuleResources::GetFileNodeByPath(const std::string& path, FileNode* node)
{
	FileNode* to_search = nullptr;
	if (App->StringCmp(node->path.data(), path.data())) {
		return node;
	}

	for (uint i = 0; i < node->children.size(); ++i) {
		if (node->children[i] != nullptr) {
			to_search = GetFileNodeByPath(path, node->children[i]);
			if (to_search != nullptr)
				break;
		}
	}
	return to_search;
}

void ModuleResources::ReadAllMetaData()
{
	OPTICK_EVENT();
	std::vector<std::string> files;
	std::vector<std::string> directories;

#ifndef GAME_VERSION
	// Init Textures
	App->file_system->DiscoverFiles(TEXTURES_FOLDER, files, directories);

	ReadTextures(directories, files, TEXTURES_FOLDER);

	files.clear();
	directories.clear();

	// Init Shaders
	App->file_system->DiscoverFiles(SHADERS_FOLDER, files, directories);
	ReadShaders(directories, files, SHADERS_FOLDER);
	files.clear();
	directories.clear();
	default_shader = GetShaderByName("default_shader");
	shadow_shader = GetShaderByName("simple_depth_shader");
	default_particle_shader = GetShaderByName("particle_shader");
	skybox_shader = GetShaderByName("skybox_shader");
	water_shader = GetShaderByName("water_shader");
	shield_fresnel_shader = GetShaderByName("shield_fresnel_shader");

	// Init Materials
	App->file_system->DiscoverFiles(MATERIALS_FOLDER, files, directories);
	ReadMaterials(directories, files, MATERIALS_FOLDER);
	files.clear();
	directories.clear();

	// Init Models & Meshes
	App->file_system->DiscoverFiles(MODELS_FOLDER, files, directories);

	ReadModels(directories, files, MODELS_FOLDER);

	files.clear();
	directories.clear();

	// Init Controllers
	App->file_system->DiscoverFiles(ANIM_CONTROLLER_FOLDER, files, directories);
	ReadAnimControllers(directories, files, ANIM_CONTROLLER_FOLDER);

	files.clear();
	directories.clear();


	// Init Prefabs
	App->file_system->DiscoverFiles(ASSETS_PREFAB_FOLDER, files, directories);
	ReadPrefabs(directories, files, ASSETS_PREFAB_FOLDER);

	files.clear();
	directories.clear();

	// Init Fonts
	App->file_system->DiscoverFiles(FONTS_FOLDER, files, directories);
	ReadFonts(directories, files, FONTS_FOLDER);

	files.clear();
	directories.clear();

	// Init Scripts
	ReadScripts();

	// Init Scenes
	App->file_system->DiscoverFiles(SCENE_FOLDER, files, directories);
	ReadScenes(directories, files, SCENE_FOLDER);

	files.clear();
	directories.clear();

	// Init Audio
	App->file_system->DiscoverFiles(AUDIO_FOLDER, files, directories);

	App->audio->LoadBanksInfo();
	ReadAudio(directories, files, AUDIO_FOLDER);

	files.clear();
	directories.clear();
#else

	// textures
	App->file_system->DiscoverFiles(LIBRARY_TEXTURES_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceTexture* texture = new ResourceTexture();
		texture->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// shaders
	App->file_system->DiscoverFiles(LIBRARY_SHADERS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceShader* shader = new ResourceShader();
		shader->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();
	default_shader = (ResourceShader*)GetResourceWithID(2074311779325559006);
	skybox_shader = (ResourceShader*)GetResourceWithID(10031399484334738574); // TODO
	//TODOSHADOW:
	shadow_shader = (ResourceShader*)GetResourceWithID(5088601162293274710);
	default_particle_shader = (ResourceShader*)GetResourceWithID(2017390725125490915);
	shield_fresnel_shader = (ResourceShader*)GetResourceWithID(5257671272918645017);
	shield_shader = (ResourceShader*)GetResourceWithID(15018513288750837760);
	default_particle_shader->SetName("particle_shader");
	default_particle_shader->TryToSetShaderType();
	shield_fresnel_shader->SetName("shield_fresnel_shader");
	shield_fresnel_shader->TryToSetShaderType();
	shield_shader->SetName("shield_shader");
	shield_shader->TryToSetShaderType();

	// materials
	App->file_system->DiscoverFiles(LIBRARY_MATERIALS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceMaterial* material = new ResourceMaterial();
		material->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// models
	App->file_system->DiscoverFiles(LIBRARY_MODELS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceModel* model = new ResourceModel();
		model->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// anim controllers
	App->file_system->DiscoverFiles(LIBRARY_ANIM_CONTROLLERS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceAnimatorController* anim_ctrl = new ResourceAnimatorController();
		anim_ctrl->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// scenes
	App->file_system->DiscoverFiles(LIBRARY_SCENES_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceScene* scene = new ResourceScene();
		scene->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// prefabs
	App->file_system->DiscoverFiles(LIBRARY_PREFABS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourcePrefab* prefab = new ResourcePrefab();
		prefab->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();

	// audio
	App->file_system->DiscoverFiles(LIBRARY_AUDIO_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceAudio* audio = new ResourceAudio();
		audio->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();
	// fonts
	App->file_system->DiscoverFiles(LIBRARY_FONTS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceFont* font = ResourceFont::LoadFile(files[i].data(), std::stoull(App->file_system->GetBaseFileName(files[i].data()).data()));
		AddResource(font);
	}
	files.clear();
	directories.clear();

	// scripts
	App->file_system->DiscoverFiles(LIBRARY_SCRIPTS_FOLDER, files, directories, true);
	for (uint i = 0; i < files.size(); ++i) {
		ResourceScript* script = new ResourceScript();
		script->ReadLibrary(files[i].data());
	}
	files.clear();
	directories.clear();
#endif
}

void ModuleResources::ReadTextures(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourceTexture* texture = new ResourceTexture();
		if (!texture->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			u64 id = texture->GetID();
			texture->CreateMetaData(id);
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadTextures(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadShaders(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourceShader* shader = new ResourceShader();
		if (!shader->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			u64 id = shader->GetID();
			shader->CreateMetaData(id);
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;

			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadShaders(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadMaterials(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourceMaterial* material = new ResourceMaterial();
		if (!material->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			/*u64 id = material->GetID();
			material->CreateMetaData(id);*/
		}
	}
	if (!directories.empty()) {
		std::vector<std::string> new_files;
		std::vector<std::string> new_directories;

		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;

			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadMaterials(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadModels(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourceModel* model = new ResourceModel();
		if (!model->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			App->importer->ReImportModel(model);
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadModels(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadAnimControllers(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourceAnimatorController* anim_ctrl = new ResourceAnimatorController();
		if (!anim_ctrl->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			u64 id = anim_ctrl->GetID();
			anim_ctrl->ReImport(id);
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadAnimControllers(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadPrefabs(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		ResourcePrefab* model = new ResourcePrefab();
		if (!model->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
			LOG_ENGINE("Error while loading %s because has not .alienPrefab", files[i]);
			delete model;
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadPrefabs(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadScenes(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		if (files[i].find("_meta.alien") == std::string::npos) {
			ResourceScene* scene = new ResourceScene();
			if (!scene->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
				LOG_ENGINE("Error loading %s", files[i].data());
				delete scene;
			}
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadScenes(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadAudio(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	auto banks = App->audio->GetBanks();
	for (uint i = 0; i < files.size(); ++i) {
		if (files[i].find("_meta.alien") == std::string::npos) {
			if (files[i].find(".bnk") != std::string::npos) {
				ResourceAudio* audio = new ResourceAudio();
				if (!audio->ReadBaseInfo(std::string(current_folder + files[i]).data())) {
					for (auto b = banks.begin(); b != banks.end(); ++b) {
						if (files[i].compare((*b)->name + ".bnk") == 0) {
							audio->CreateMetaData((*b)->id);
						}
					}
				}
			}
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadAudio(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadFonts(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder)
{
	for (uint i = 0; i < files.size(); ++i) {
		if (files[i].find("_meta.alien") == std::string::npos) {
			std::string path = App->file_system->GetPathWithoutExtension(std::string(current_folder + files[i]).data()) + "_meta.alien";
			ResourceFont* font = ResourceFont::ImportFile(std::string(current_folder + files[i]).data(), GetIDFromAlienPath(path.data()));
		}
	}
	if (!directories.empty()) {
		for (uint i = 0; i < directories.size(); ++i) {
			std::vector<std::string> new_files;
			std::vector<std::string> new_directories;
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories);
			ReadFonts(new_directories, new_files, dir);
		}
	}
}

void ModuleResources::ReadScripts()
{
#ifndef GAME_VERSION
	std::vector<std::string> files;
	std::vector<std::string> directories;
	std::vector<std::string> scripts;

	App->file_system->DiscoverFiles(SCRIPTS_FOLDER, files, directories, true);
	GetAllScriptsPath(directories, files, SCRIPTS_FOLDER, &scripts);

	files.clear();
	directories.clear();

	App->file_system->DiscoverFiles(HEADER_SCRIPTS_FILE, files, directories);

	for (uint i = 0; i < files.size(); ++i) {
		if (files[i].back() == 'h') { // header file found
			ReadHeaderFile(std::string(HEADER_SCRIPTS_FILE + files[i]).data(), scripts);
		}
	}
	
	// if there are still scripts, remove them because it doesnt exists in the project
	if (!scripts.empty()) {
		for (uint i = 0; i < scripts.size(); ++i) {
			remove(scripts[i].data());
		}
		scripts.clear();
	}
#else
	std::vector<std::string> files;
	std::vector<std::string> directories;
	std::vector<std::string> scripts;
	
	App->file_system->DiscoverFiles(SCRIPTS_FOLDER, files, directories, true);
	GetAllScriptsPath(directories, files, SCRIPTS_FOLDER, &scripts);

	files.clear();
	directories.clear();

	for (uint i = 0; i < scripts.size(); ++i) {
		ResourceScript* script = new ResourceScript();
		script->SetName(App->file_system->GetBaseFileName(scripts[i].data()).data());
		script->ReadBaseInfo(scripts[i].data());
	}
#endif
}

void ModuleResources::GetAllScriptsPath(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder, std::vector<std::string>* scripts)
{
	if (!files.empty()) {
		scripts->insert(scripts->end(), files.begin(), files.end());
	}
	if (!directories.empty()) {
		std::vector<std::string> new_files;
		std::vector<std::string> new_directories;

		for (uint i = 0; i < directories.size(); ++i) {
			std::string dir = current_folder + directories[i] + "/";
			App->file_system->DiscoverFiles(dir.data(), new_files, new_directories, true);
			GetAllScriptsPath(new_directories, new_files, dir, scripts);
		}
	}
}

void ModuleResources::CreateAsset(FileDropType type)
{
	switch (type)
	{
	case FileDropType::ANIM_CONTROLLER:
		CreateAnimatorController();
		break;
	case FileDropType::ANIMATION:
		break;

	}

	App->ui->panel_project->RefreshAllNodes();
}

void ModuleResources::CreateAnimatorController()
{
	std::string asset_name = "NewAnimatorController";
	App->ui->panel_project->GetUniqueFileName(asset_name, ANIM_CONTROLLER_FOLDER);

	ResourceAnimatorController* new_controller = new ResourceAnimatorController();
	new_controller->name = asset_name;
	new_controller->SaveAsset();
}

void ModuleResources::HandleEvent(EventType eventType)
{
	switch (eventType)
	{
	case EventType::ON_SAVE:
		for (std::vector<Resource*>::iterator iter = resources.begin(); iter != resources.end(); ++iter) {
			(*iter)->SaveResource();
		}
		break;

	default:
		break;
	}
}

void ModuleResources::HandleAlienEvent(const AlienEvent& alienEvent)
{

	switch (alienEvent.type)
	{
	case AlienEventType::RESOURCE_SELECTED:
		{
			Resource* resource = static_cast<Resource*>(alienEvent.object);
			if (resource != nullptr)
				resource->OnSelected();
		} break;

	case AlienEventType::RESOURCE_DESELECTED:
		{
			Resource* resource = static_cast<Resource*>(alienEvent.object);
			if (resource != nullptr)
				resource->OnDeselected();
		} break;

	default:
		break;
	}
}

ResourceMaterial* ModuleResources::CreateMaterial(const char* name, const char* folderPath)
{
	if (!App->file_system->IsPathInsideOtherPath(folderPath, MATERIALS_FOLDER))
		folderPath = MATERIALS_FOLDER;

	std::string materialName = name;
	App->ui->panel_project->GetUniqueFileName(materialName, folderPath);
	
	ResourceMaterial* new_material = new ResourceMaterial();
	new_material->SetName(materialName.c_str());
	new_material->SetAssetsPath(std::string(folderPath + materialName + ".material").data());
	new_material->SaveResource();
	App->ui->panel_project->RefreshAllNodes();

	return new_material;
}





