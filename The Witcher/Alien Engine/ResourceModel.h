#pragma once

#include "Resource_.h"
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"

class ResourceMesh;
class ResourceAnimation;
class ResourceBone;
class GameObject;
class ResourceMaterial;

struct ModelNode {
	std::string	name;
	std::string	parent_name;
	float3 pos = { 0,0,0 };
	float3 scale = { 1,1,1 };
	Quat rot = Quat::identity();
	uint node_num = 0;
	uint parent_num = 0;
	int mesh = -1;
	int material = -1; 
	int bone = -1;
};

class ResourceModel : public Resource {

	friend class ModuleImporter;

public:

	ResourceModel();
	virtual ~ResourceModel();

	// meta data
	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
	bool LoadMemory();
	bool DeleteMetaData();
	void UpdateAnimationInfo();

	// create GameObjects
	void ConvertToGameObjects();

private:

	// sort
	static bool SortByFamilyNumber(const ModelNode& node1, const ModelNode& node2);

	GameObject* CreateGameObject(const ModelNode& node, std::vector<std::pair<uint, GameObject*>>& objects_created, std::pair<GameObject*, GameObject*>& skeleton_link);

public:
	std::vector<ModelNode> model_nodes;
	std::vector<ResourceMesh*> meshes_attached;
	std::vector<ResourceAnimation*> animations_attached;
	std::vector<ResourceBone*> bones_attached;
	std::vector<ResourceMaterial*> materials_attached;
};