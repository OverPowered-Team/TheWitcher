#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

#define RADIUS_INTENSITY_MULTIPLIE_POINT 45

struct __declspec(dllexport) PointLightProperties
{
	bool enabled = true;
	float intensity = 1.0f;
	float3 position = float3::zero();
	float3 ambient = float3::zero();
	float3 diffuse = float3::one();
	float3 specular = float3::one();
	float constant = 1.0f;
	float linear = 0.1f;
	float quadratic = 0.02f;

	Component* light = nullptr;

	bool isEnabled() const
	{
		return light->game_object_attached->IsEnabled() && enabled;
	}
};

class __declspec(dllexport) ComponentLightPoint : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightPoint(GameObject* attach);
	virtual ~ComponentLightPoint();

private:
	void LightLogic();
	void Update() override;
	void DrawScene() override; 

	bool DrawInspector();
	void OnEnable();
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

public:
	PointLightProperties light_props;
};
