#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;

class __declspec(dllexport) ComponentMeshCollider : public ComponentCollider
{
	friend class GameObject;

public:
	ComponentMeshCollider(GameObject* go);
private:
	void ScaleChanged();
	void DrawSpecificInspector();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void Clone(Component* clone);
	void Reset();

	bool CreateMeshShape();

private:
	PxTriangleMesh* triangle_mesh = nullptr;
};

