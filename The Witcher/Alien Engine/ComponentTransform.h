#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class __declspec(dllexport) ComponentTransform : public Component {
	friend class CompZ;
	friend class PanelScene;
	friend class GameObject;
	friend class ComponentMesh;
	friend class ComponentAnimator;
	friend class ComponentUI;
	friend class ComponentText;
	friend class ComponentCheckbox;
	friend class ComponentButton;
	friend class ComponentBar;
	friend class ComponentCanvas;
	friend class ComponentAnimatedImage;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentRigidBody;
	friend class ModuleCamera3D;
	friend class ModuleObjects;
	friend class ComponentDeformableMesh;
	friend class ResourceModel;
	friend class ModuleUI;
	friend class ComponentSlider;
	friend class PanelInspector;
public:

	ComponentTransform(GameObject* attach);
	ComponentTransform(GameObject* attach, const float3& pos, const Quat& rot, const float3& scale);
	virtual ~ComponentTransform();

	void SetLocalPosition(const float3& new_local_pos);
	void SetLocalPosition(const float& x, const float& y, const float& z);

	const float3 GetLocalPosition() const;
	void SetGlobalPosition(const float3& pos);
	const float3 GetGlobalPosition() const;

	void SetLocalScale(const float3& new_local_scale);
	void SetLocalScale(const float& x, const float& y, const float& z);

	void SetLocalTransform(const float3& position, const Quat& rotation, const float3& scale);
	
	const float3 GetLocalScale() const;
	const float3 GetGlobalScale() const;

	void SetLocalRotation(const Quat& new_local_rotation);
	void SetLocalRotation(const float& x, const float& y, const float& z, const float& angle);
	
	void SetGlobalRotation(Quat rotation);
	const Quat GetLocalRotation() const;
	const Quat GetGlobalRotation() const;

	float4x4 GetGlobalMatrix() const;
	void AddPosition(const float3 pos);
	void AddScale(const float3 scale);
	void AddRotation(const float3 rot);

private:

	void LookScale();
	void RecalculateTransform();

	void Reparent(const float4x4& transform);

	bool DrawInspector();

	bool AddNewTagClicked(const char* new_tag);

	void SetScaleNegative(const bool& negative);
	bool IsScaleNegative();

	void Reset();
	void SetComponent(Component* component);

	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void SetGlobalTransformation(const float4x4& global_transformation);

public:

	float3 forward = { 0,0,0 };
	float3 up = { 0,0,0 };
	float3 right = { 0,0,0 };

private:

	float4x4 global_transformation = float4x4::identity();
	float4x4 local_transformation = float4x4::identity();
	
	// to know if flip poly or not
	bool is_scale_negative = false;
	// position
	float3 local_position = { 0,0,0 };
	// sacale
	float3 local_scale = { 0,0,0 };
	// rotation
	Quat local_rotation = { 0,0,0,0 };
	
	float3 euler_rotation = { 0,0,0 };

	bool popup_static = false;
	bool popup_tags = false;
};
