//#pragma once
//
//#include "C_Collider.h"
//#include "external/MathGeoLib/include/Math/MathAll.h"
//#include "Bullet/include/btBulletDynamicsCommon.h"
//
//#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"
//
//class GameObject;
//class ModulePhysics;
//class ComponentMesh;
//
//class ComponentConvexHullCollider : public ComponentCollider
//{
//public:
//
//	friend ModulePhysics;
//
//public:
//
//	ComponentConvexHullCollider(GameObject* go);
//	~ComponentConvexHullCollider();
//
//	// Virtual Functions ------------------------------
//
//	bool Render();
//
//	void CreateShape(C_Mesh* mesh);
//
//	void AdjustShapeToAABB();
//
//	void SaveCollider(Config& config);
//
//	void LoadCollider(Config& config);
//
//	void DrawPanelColliderInfo();
//
//private:
//
//	float3 CheckInvalidCollider(float3 size);
//
//
//private:
//
//	float3 size;
//	btShapeHull* hull = nullptr;
//
//};
