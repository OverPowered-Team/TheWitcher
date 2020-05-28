#pragma once

#include "ComponentBasePhysic.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "extensions/PxJoint.h"

class GameObject;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class Alien;

using namespace physx;

typedef ComponentRigidBody RigidBody;

class __declspec(dllexport) ComponentJoint : public ComponentBasePhysic
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ComponentPhysics;
	friend class ComponentRigidBody;

public:

	ComponentJoint(GameObject* go);
	virtual ~ComponentJoint();

	void SetConnectedBody(RigidBody* body);
	RigidBody* GetConnectedBody() { return connected_body; }

	void SetAnchor( float3 _anchor);
	float3 GetAnchor() { return anchor; }

protected:

	void CreateJoint();
	void DestroyJoint();

	virtual PxJoint* CreateSpecificJoint() = 0;
	virtual void	 SetupSpecificJoint() = 0;

	void DrawScene();
	bool DrawInspector();
	void HandleAlienEvent(const AlienEvent& e);

	virtual void Reset() {}
	virtual void DrawInspectorJoint() = 0;
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

protected:

	RigidBody*	connected_body = nullptr;
	PxJoint*	joint = nullptr;
	float3		anchor = float3::zero();
	float3		axis = float3(1,0,0);
	float3		connected_anchor = float3::zero();
	float3		second_axis = float3(0, 1, 0);
};
