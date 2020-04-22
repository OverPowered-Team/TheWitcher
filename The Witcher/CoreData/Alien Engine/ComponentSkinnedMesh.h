#pragma once

#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"
#include <vector>
#include "MathGeoLib/include/Geometry/AABB.h"

class ResourceMesh;
class ComponentBone;
class ComponentTransform;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameobject);
	virtual ~ComponentMesh();

	void Update();
	void Draw();
	void AddMesh(ResourceMesh* tex);

	void SetDebugVertexNormal();
	void SetDebugFaceNormal();
	void SetDebugSkeleton(bool value);

	void SetDebugBoundingBox();

	int GetVerticesAmount();
	int GetNormalsAmount();
	int GetUVAmount();
	int GetIndicesAmount();

	AABB GetAABB();

	//bool Save(const nlohmann::json::iterator& it);
	//bool Load(const nlohmann::json comp);

	ResourceMesh* GetMesh();
	void AttachSkeleton(ComponentTransform* root);
	void AttachSkeleton();

private:
	void AttachBone(ComponentTransform* bone_transform);
	void UpdateDeformableMesh();

private:
	ResourceMesh* mesh = nullptr;
	ResourceMesh* deformable_mesh = nullptr;
	std::vector<ComponentBone*> bones;

public:
	bool debug_skeleton = false, debug_bounding_box = false;
	bool to_draw = false;

	uint root_bone_id = 0;
};

#endif // !_COMPONENT_MESH_H_

