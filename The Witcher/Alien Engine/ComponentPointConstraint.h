#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class Alien;

class __declspec(dllexport) ComponentPointConstraint : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentPointConstraint(GameObject* go);
	virtual ~ComponentPointConstraint();

protected:

	void Update();
	void DrawScene(ComponentCamera* camera) override;
	bool DrawInspector();

	void RecreateConstraint();

	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

protected:

	GameObject* connected_body = nullptr; // both gameobjects need a physics rigidbody
	u64 connected_body_id = 0;
	ComponentTransform* transform = nullptr;

	float3 pivotA;
	float3 pivotB;
	//btPoint2PointConstraint* constraint = nullptr;
	bool disable_collision = false;
};