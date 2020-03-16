#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;


class ComponentCapsuleCollider : public ComponentCollider
{
public:

	friend class GameObject;
	friend class ModulePhysics;

public:

	enum class CapsuleType
	{
		X = 0, Y = 1, Z = 2
	};

	ComponentCapsuleCollider(GameObject* go);

	void SetRadius(float radius);
	float GetRadius() { return radius; }
	void SetHeight(float height);
	float GetHeight() { return height; }

private:

	void DrawSpecificInspector();

	void Reset();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void CreateDefaultShape();
	void UpdateShape();

private:

	float final_radius = 0.5f;
	float final_height = 1.f;
	float radius = 0.5f;
	float height = 1.f;

	CapsuleType capsule_type = CapsuleType::Y;

};
