#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

struct __declspec(dllexport) DirLightProperties
{
	float intensity = 1.0f;
	float3 position = float3::zero();
	float3 direction = float3::zero();
	float3 ambient = float3::zero();
	float3 diffuse = float3::one();
	float3 specular = float3::one();
};

class __declspec(dllexport) ComponentLightDirectional : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightDirectional(GameObject* attach);
	virtual ~ComponentLightDirectional();

private:
	void LightLogic();

	bool DrawInspector();
	void OnDisable();

	void Clone(Component* clone);

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void DrawIconLight();

private:
	ComponentMesh* bulb = nullptr;
	bool print_icon = true;

	uint renderer_id = 0;

	DirLightProperties light_props;
};
