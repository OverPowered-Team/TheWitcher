#pragma once
#include "ComponentMesh.h"
#include <vector>

class ResourceMesh;
class ComponentBone;

class ComponentDeformableMesh : public ComponentMesh
{
	friend class GameObject;
	friend class ReturnZ;
public:
	ComponentDeformableMesh(GameObject* attach);
	virtual ~ComponentDeformableMesh();

	void AttachSkeleton(ComponentTransform* root);
	void AttachSkeleton();

protected:
	void AttachBone(ComponentTransform* bone_transform);
	void DeformMesh();
	void DrawPolygon(ComponentCamera* camera);
	
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	ResourceMesh* original_mesh = nullptr;
	std::vector<ComponentBone*> bones;
	u64 root_bone_id = 0;
};
