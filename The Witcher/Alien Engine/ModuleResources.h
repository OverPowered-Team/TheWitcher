#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleObjects.h"

#define DROP_ID_HIERARCHY_NODES "hierarchy_node"
#define DROP_ID_PROJECT_NODE "project_node"

class FileNode;
enum class FileDropType;

class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceScene;
class ResourceTexture;
class ResourceShader;
class ResourceMaterial;
class ResourceFont;

struct Icons {

	ResourceTexture* jpg_file = nullptr;
	ResourceTexture* png_file = nullptr;
	ResourceTexture* dds_file = nullptr;
	ResourceTexture* tga_file = nullptr;
	ResourceTexture* shader_file = nullptr;
	ResourceTexture* script_file = nullptr;
	ResourceTexture* folder = nullptr;
	ResourceTexture* prefab_icon = nullptr;
	ResourceTexture* model = nullptr;
	ResourceTexture* return_icon = nullptr;
	ResourceTexture* scene_file = nullptr;
	ResourceTexture* move_transform = nullptr;
	ResourceTexture* rotate_transform = nullptr;
	ResourceTexture* scale_transform = nullptr;
	ResourceTexture* undo = nullptr;
	ResourceTexture* redo = nullptr;
	ResourceTexture* global = nullptr;
	ResourceTexture* local = nullptr;
	ResourceTexture* play = nullptr;
	ResourceTexture* pause = nullptr;
	ResourceTexture* once = nullptr;
	ResourceTexture* box = nullptr;
	ResourceTexture* prefab = nullptr;
	ResourceTexture* prefab_lock = nullptr;
};

class ModuleResources : public Module
{
public:

	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddResource(Resource* resource);
	void RemoveResource(Resource* resource);
	void CreateAsset(FileDropType type);

	// meta path
	bool CreateNewModelInstanceOf(const char* path);

	u64 GetIDFromAlienPath(const char* path);
	Resource* GetResourceWithID(const u64& ID);
	const Resource* GetResourceWithID(const u64& ID) const;
	std::vector<Resource*> GetResourcesWithType(ResourceType type);

	void AddNewFileNode(const std::string& path, bool is_file);

	u64 GetRandomID();

	ResourceTexture* GetTextureByName(const char* name);
	const ResourceTexture* GetTextureByName(const char* name) const;

	ResourceMesh* GetPrimitive(const PrimitiveType& type);
	const ResourceMesh* GetPrimitive(const PrimitiveType& type) const;

	bool Exists(const char* path, Resource** resource) const;

	void CreatePrimitive(const PrimitiveType& type, ResourceMesh** mesh);
	void CreatePrimitive(const PrimitiveType& type, ResourceMesh** mesh) const;

	void ReadHeaderFile(const char* path, std::vector<std::string>& current_scripts);
	void ReloadScripts();

	ResourceScene* GetSceneByName(const char* name);
	ResourceFont* GetFontByName(const char* name);
	ResourceShader* GetShaderByName(std::string shaderName);
	ResourceMaterial* GetMaterialByName(const char* name); 

	const uint GetTextureidByID(const u64& ID) const;
	ResourceTexture* GetTextureByID(const u64& ID);

	// Shaders
	bool GetShaders(std::vector<ResourceShader*>& to_fill);
	ResourceMaterial* CreateMaterial(const char* name, const char* folderPath = "Assets/Materials/" /*MATERIALS_FOLDER*/);

	void HandleEvent(EventType eventType) override;
	void HandleAlienEvent(const AlienEvent& alienEvent);

private:
	FileNode* GetFileNodeByPath(const std::string& path, FileNode* node);

	void ReadAllMetaData();
	void ReadTextures(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadShaders(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadMaterials(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadModels(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadAnimControllers(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadPrefabs(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadScenes(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadAudio(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadFonts(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder);
	void ReadScripts();

	void GetAllScriptsPath(std::vector<std::string> directories, std::vector<std::string> files, std::string current_folder, std::vector<std::string>* scripts);

	void CreateAnimatorController();


public:

	Icons icons;

	std::vector<Resource*> resources;

	ResourceMesh* camera_mesh = nullptr;
	ResourceMesh* light_mesh = nullptr; 
	ResourceFont* default_font = nullptr;
	ResourceMaterial* default_material = nullptr;
	ResourceShader* simple_depth_shader = nullptr;
	ResourceShader* default_particle_shader = nullptr;
	ResourceShader* skybox_shader = nullptr;
	ResourceShader* default_shader = nullptr;
	ResourceShader* water_shader = nullptr;
	ResourceShader* shield_fresnel_shader = nullptr;
	ResourceShader* shield_shader = nullptr;
	FileNode* assets = nullptr;

private:
	ResourceMesh* cube = nullptr;
	ResourceMesh* sphere = nullptr;
	ResourceMesh* rock = nullptr;
	ResourceMesh* torus = nullptr;
	ResourceMesh* dodecahedron = nullptr;
	ResourceMesh* icosahedron = nullptr;
	ResourceMesh* octahedron = nullptr;
};


