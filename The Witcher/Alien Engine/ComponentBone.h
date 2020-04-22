#ifndef _COMPONENT_BONE_H_
#define _COMPONENT_BONE_H_

#include "Component.h"

class ResourceBone;
class ComponentTransform;

class __declspec(dllexport) ComponentBone : public Component
{
	friend class GameObject;
	friend class ResourceModel;
public:
	ComponentBone(GameObject* gameobject);
	~ComponentBone();
public:
	bool debug_draw = false;
	void Draw();
	void AddBone(ResourceBone* b);
	std::string GetMeshName();
	uint GetBoneID();
	ResourceBone* GetBone();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	ResourceBone* bone = nullptr;
};

#endif // !_COMPONENT_BONE_H_



