#pragma once

#include "ComponentJoint.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class Alien;

class __declspec(dllexport) ComponentConfigurableJoint : public ComponentJoint
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentConfigurableJoint(GameObject* go);

protected:

	PxJoint* CreateSpecificJoint();
	void SetupSpecificJoint();
	void DrawInspectorJoint();
	void CloneJoint(Component* clone) {}
	void SaveComponentJoint(JSONArraypack* to_save);
	void LoadComponentJoint(JSONArraypack* to_load);

};