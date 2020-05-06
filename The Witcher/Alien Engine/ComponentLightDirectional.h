#pragma once

#include "Component.h"
#include "Color.h"
#include "ComponentMesh.h"
#include "ModuleObjects.h"

struct __declspec(dllexport) DirLightProperties
{
	bool enabled = true;
	float intensity = 1.0f;
	float3 position = float3::zero();
	float3 fake_position = float3::zero();
	float3 fake_position_baked[3];
	float3 direction = float3::zero();
	float3 ambient = {0.76F,0.76F,0.76F };
	float3 diffuse = float3::one();
	float3 specular = float3::one();

	ComponentLightDirectional* light = nullptr;
	uint depthMap = 0;
	uint depthMapFBO;

	uint bakedepthMap[3];
	uint bakedepthMapFBO;

	float4x4 viewMat;
	float4x4 projMat;
};

class __declspec(dllexport) ComponentLightDirectional : public Component {
	friend class GameObject;
	friend class ComponentMesh;
public:
	ComponentLightDirectional(GameObject* attach);
	void InitFrameBuffers();
	virtual ~ComponentLightDirectional();

	void PostUpdate();

	void BindForWriting(uint cascadeIndex);
	void BindForReading();
	void CalculateBakedViewMatrix();

private:
	void LightLogic();
	void DrawScene(ComponentCamera* camera) override;


	bool DrawInspector();
	void OnEnable();
	void OnDisable();

	void Clone(Component* clone);

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void DrawIconLight();

	uint num_of_static_shadowMap = 3;

public:
	float sizefrustrum = 88.f;
	float distance_far_plane = 178.f;
	float sizefrustrumbaked = 153.f;
	float3 fake_position;
	bool castShadows = true;
	bool bakeShadows = true;

	float4x4 projMatrix;
	float4x4 viewMatrix[3];

private:
	ComponentMesh* bulb = nullptr;
	bool print_icon = true;

	uint renderer_id = 0;
	DirLightProperties light_props;
};