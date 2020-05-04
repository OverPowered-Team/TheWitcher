#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "Color.h"

class ResourceMesh;
class ResourceMaterial;

class __declspec(dllexport) ComponentMesh : public Component {

	friend class ReturnZ;
	friend class CompZ;
	friend class ResourceModel;
	friend class ResourceMesh;
	friend class ComponentLightDirectional;
	friend class ComponentLightSpot;
	friend class ComponentLightPoint;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentCamera;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentConvexHullCollider;
	friend class GameObject;
	friend class ModuleCamera3D;
	friend class ModuleObjects;
	friend class Gizmos;
	friend class Octree;
	friend class OctreeNode;
	friend class ResourceModel;
	friend class PanelCreateObject;
	friend class PanelRender;

public:
	ComponentMesh(GameObject* attach);
	virtual ~ComponentMesh();

	const AABB GetGlobalAABB() const;
	const AABB GetLocalAABB() const;

protected:

	void SetResourceMesh(ResourceMesh* resource);

	void DrawScene(ComponentCamera* camera) override;
	void DrawGame(ComponentCamera* camera) override;

	virtual void DrawPolygon(ComponentCamera* camera);
	virtual void PreDrawPolygonForShadows(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position);
	virtual void DrawOutLine();
	virtual void DrawMesh();

	virtual void SetUniforms(ResourceMaterial* resource_material, ComponentCamera* camera);
	virtual void SetShadowUniforms(ResourceMaterial* resource_material, ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position);


	void DrawVertexNormals();
	void DrawFaceNormals();
	bool DrawInspector();
	void DrawGlobalAABB(ComponentCamera* camera);
	void DrawOBB(ComponentCamera* camera);

	void Reset();
	void SetComponent(Component* component);

	void Clone(Component* clone);

	void GenerateLocalAABB();
	void RecalculateAABB_OBB();

	const OBB GetOBB() const;

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

protected:
	
	ResourceMesh* mesh = nullptr;

	bool view_mesh = false;
	bool wireframe = false;
	bool view_vertex_normals = false;
	bool view_face_normals = false;

	bool draw_AABB = false;
	bool draw_OBB = false;

	AABB local_aabb;
	OBB obb;  
	AABB global_aabb;
	int animate = 0;
};