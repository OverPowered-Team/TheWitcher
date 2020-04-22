#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;
class ComponentMesh;
class btShapeHull;

class __declspec(dllexport)  ComponentConvexHullCollider : public ComponentCollider
{
	friend class GameObject;

public:

	ComponentConvexHullCollider(GameObject* go);

private:

	void Update();

	void DrawSpecificInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* component);

	void ScaleChanged();


	PxShape* CreateConvexMesh(const GameObject* go);

private:

	ComponentMesh* mesh = nullptr;
	float3 size = float3::zero();

	int vertex_limit = 64.0f; // default vertex limit
	float3 prev_scale;
	bool valid = false; // used only to avoid getting on each frame the mesh to print error message on panel

};
