#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class btKinematicCharacterController;
class Alien;

class __declspec(dllexport) ComponentCharacterController : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentCharacterController(GameObject* go);
	virtual ~ComponentCharacterController();

protected:

	void Update();
	void DrawScene();
	bool DrawInspector();

	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

protected:
	ComponentTransform* transform = nullptr;
	btKinematicCharacterController* controller = nullptr;
	btPairCachingGhostObject* body = nullptr;
	btCapsuleShape* shape = nullptr;
};