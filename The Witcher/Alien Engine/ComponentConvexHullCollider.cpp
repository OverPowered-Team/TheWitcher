#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentConvexHullCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "mmgr/mmgr.h"

#include "ComponentPhysics.h"

ComponentConvexHullCollider::ComponentConvexHullCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Convex Hull Collider");
	type = ComponentType::CONVEX_HULL_COLLIDER;
	// stores current gameobject scale
	//prev_scale = float3(transform->GetGlobalScale());

	shape = CreateConvexMesh(go);
	if (!shape) {	// if convex mesh cook fail, create default cube
		shape = App->physx->CreateShape(PxBoxGeometry(.5f, .5f, .5f), *material);
	}

	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);

	InitCollider();
}

PxShape* ComponentConvexHullCollider::CreateConvexMesh(const GameObject* go)
{
	// try to get mesh from gameobject
	const ComponentMesh* mesh = GetMesh();
	if (!mesh) {
		valid = false;
		return nullptr;
	}

	if (shape) // if we are re-creating actual shape
		BeginUpdateShape();

	uint nverts = mesh->mesh->num_vertex;
	uint ntris = mesh->mesh->num_faces;
	uint nindex = mesh->mesh->num_index;

	PxConvexMeshDesc convexDesc;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.points.count = nverts;
	convexDesc.points.stride = sizeof(float) * 3;
	convexDesc.points.data = mesh->mesh->vertex;
	convexDesc.vertexLimit = (uint)vertex_limit;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;

	if (!App->physx->px_cooking->cookConvexMesh(convexDesc, buf, &result))
		return nullptr;

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = App->physx->px_physics->createConvexMesh(input);

	valid = true;
	if (shape)	// if we are re-creating actual shape
	{
		PxTransform trans = shape->getLocalPose();
		float3 rad_rot = float3(rotation.x, rotation.y, rotation.z) * DEGTORAD;
		trans.p = F3_TO_PXVEC3(center.Mul(transform->GetGlobalScale()));
		trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rot.x, rad_rot.y, rad_rot.z));
		shape->setLocalPose(trans);
		shape->setGeometry(PxConvexMeshGeometry(convexMesh, PxMeshScale(F3_TO_PXVEC3(transform->GetGlobalScale())), PxConvexMeshGeometryFlag::eTIGHT_BOUNDS));
		EndUpdateShape();
		return shape;
	}
	
	return App->physx->CreateShape(PxConvexMeshGeometry(convexMesh, PxMeshScale(F3_TO_PXVEC3(transform->GetGlobalScale())), PxConvexMeshGeometryFlag::eTIGHT_BOUNDS), *material);
}


void ComponentConvexHullCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("VertexLimit", vertex_limit);

}

void ComponentConvexHullCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	vertex_limit = to_load->GetNumber("VertexLimit");
	CreateConvexMesh(game_object_attached);
}

void ComponentConvexHullCollider::Update()
{
}

void ComponentConvexHullCollider::Clone(Component* clone)
{
	ComponentConvexHullCollider* hull_clone = (ComponentConvexHullCollider*)clone;
	hull_clone->vertex_limit = vertex_limit;
	hull_clone->center = center;
	hull_clone->rotation = rotation;
	hull_clone->CreateConvexMesh(clone->game_object_attached);
}

void ComponentConvexHullCollider::ScaleChanged()
{
	CreateConvexMesh(game_object_attached);
}

void ComponentConvexHullCollider::DrawSpecificInspector()
{
	ImGui::Title("Vertex Limit", 1);
	if (ImGui::SliderInt("##vertexLimit", &vertex_limit, 8, 255))
	{
		Clamp(vertex_limit, 8, 255);
		CreateConvexMesh(game_object_attached);
	}

	if(!valid)
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2, 1.0f), "ERROR: gameobject without mesh, convex collider NOT created");
}
