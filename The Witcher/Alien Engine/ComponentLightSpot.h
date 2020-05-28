#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

#define RADIUS_INTENSITY_MULTIPLIER_SPOT 3.5f

struct __declspec(dllexport) SpotLightProperties
{
	bool enabled = true;
	float intensity = 1.0f;
	float3 position = float3::zero();
	float3 direction = float3::zero();
	float3 ambient = float3::zero();
	float3 diffuse = float3::one();
	float3 specular = float3::one();
	float constant = 1.0f;
	float linear = 0.1f;
	float quadratic = 0.01f;
	float cut_off = 1.75f;
	float outer_cut_off = 15.0f;

	Component* light = nullptr;

	bool isEnabled() const
	{
		return light->game_object_attached->IsEnabled() && enabled;
	}

};

class __declspec(dllexport) ComponentLightSpot : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightSpot(GameObject* attach);
	virtual ~ComponentLightSpot();

private:
	void Update() override;
	void DrawScene() override;
	void LightLogic();

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
	SpotLightProperties light_props;
};
