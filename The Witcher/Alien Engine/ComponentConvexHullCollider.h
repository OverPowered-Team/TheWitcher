#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class btShapeHull;

class __declspec(dllexport)  ComponentConvexHullCollider : public ComponentCollider
{
	friend class ModulePhysics;
	friend class GameObject;

public:

	ComponentConvexHullCollider(GameObject* go);
	void SetSize(float3 size);

private:

	float3 CheckInvalidCollider(float3 size);
	void DrawSpecificInspector();

	void DrawScene();
	void Clone(Component* clone);
	void Reset();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateDefaultShape();
	void UpdateShape();
	void SetCenter(float3 value);
	void SetScale(const float3 scale);

private:

	ComponentMesh* mesh = nullptr;
	float3 final_size = float3::zero();
	float3 size = float3::zero();
	btShapeHull* hull = nullptr;

};
