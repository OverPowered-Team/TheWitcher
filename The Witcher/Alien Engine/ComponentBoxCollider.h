#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;


class __declspec(dllexport) ComponentBoxCollider : public ComponentCollider
{
	friend class ModulePhysics;
	friend class GameObject;

public:

	ComponentBoxCollider(GameObject* go);

	void SetCenter(float3 center);
	void SetSize(float3 size);
	float3 GetSize() { return size; }

private:

	float3 CheckInvalidCollider(float3 size);
	void DrawSpecificInspector();

	void Reset();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	
	void CreateShape();
	void UpdateShape();
	bool WrapMesh();

private:

	float3 final_size = float3::zero();
	float3 size = float3::zero();
};

