#pragma once
#include "Component.h"

class ComponentPhysics;

class __declspec(dllexport) ComponentBasePhysic : public Component
{
	friend class ComponentBoxCollider;
	friend class ComponentPhysics;
	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class GameObject;
	friend class UserControllerHitReport;

public:

	ComponentBasePhysic(GameObject* go);
	virtual ~ComponentBasePhysic();

	virtual void Reset() {}
	virtual void Clone(Component* clone) {}

protected:

	virtual void Update(){}
	virtual void PostUpdate(){}

	virtual void OnEnable(){}
	virtual void OnDisable(){}
	virtual bool DrawInspector() { return true; }
	virtual void HandleAlienEvent(const AlienEvent& e) {}
	virtual void DrawScene(ComponentCamera* camera) override {}

	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save) {}
	virtual void LoadComponent(JSONArraypack* to_load) {}

protected:

	std::string name = "Collider";
	ComponentTransform* transform = nullptr;
	ComponentPhysics* physics = nullptr;
	GameObject* go = nullptr;
};