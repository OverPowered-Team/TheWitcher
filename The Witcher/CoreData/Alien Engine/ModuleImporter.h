#pragma once

#include "Globals.h"
#include "Module.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/mesh.h"
#include "FreeType/include/ft2build.h"
#include "FreeType/include/freetype/freetype.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc141-mtd.lib")
#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment (lib, "Devil/libx86/ILU.lib")
#pragma comment (lib, "Devil/libx86/ILUT.lib")

#pragma comment(lib, "Freetype/libx86/freetype.lib")

#include <vector>
//#include "glew/include/glew.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "Shapes.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

class ResourceModel;
class ResourceMesh;
class ResourceTexture;
class ResourceMaterial;
class ResourceShader;
enum class TextureType;
class ResourceFont;

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	// models
	bool LoadModelFile(const char* path, const char* extern_path); // when dropped
	void LoadParShapesMesh(par_shapes_mesh* p_mesh, ResourceMesh* mesh);
	ResourceMesh* LoadEngineModels(const char* path);
	bool ReImportModel(ResourceModel* model); // when dropped
	void ReImportAnimations(ResourceModel* model, const aiScene* scene);
	
	// textures
	ResourceTexture* LoadTextureFile(const char* path, bool has_been_dropped = false, bool is_custom = true); // when dropped
	ResourceTexture* LoadEngineTexture(const char* path);
	ResourceFont* LoadFontFile(const char* path);
	void LoadTextureToResource(const char* path, ResourceTexture* texture);
	void ApplyTextureToSelectedObject(ResourceTexture* texture);
	void ApplyParticleSystemToSelectedObject(std::string path); // For the moment there are no resource particle system (no need meta)

	// shaders
	ResourceShader* LoadShaderFile(const char* path, bool has_been_dropped = false, bool is_custom = true); // when dropped
	void ApplyShaderToSelectedObject(ResourceShader* shader);

	void ApplyMaterialToSelectedObject(ResourceMaterial* material);

private:
	
	// models
	void InitScene(const char* path, const aiScene* scene, const char* extern_path);

	void LoadAnimation(const aiAnimation* animation);
	void LoadBone(const aiBone* bone, std::string mesh_name);
	void LoadMesh(const aiMesh* mesh);
	void LoadNode(const aiNode* node, const aiScene* scene, uint nodeNum);
	void LoadMaterials(aiMaterial* material, const char* extern_path);
	void LoadModelTexture(const aiMaterial* material, ResourceMaterial* mat, aiTextureType assimp_type, TextureType type, const char* extern_path);

private:
	ResourceModel* model = nullptr;

public:
	FT_Library library;
};


