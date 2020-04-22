#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentSphereCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;

	InitializeRadius();
	shape = App->physx->CreateShape(PxSphereGeometry(CalculateRadius()), *material);
	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
	InitCollider();
}
void ComponentSphereCollider::InitializeRadius()
{
	radius = GetLocalMeshAabbSize().MaxElement() * 0.5f;
}

const float ComponentSphereCollider::CalculateRadius()
{
	return transform->GetGlobalScale().MaxElement() * radius;
}

PxShape* ComponentSphereCollider::RecreateSphereShape()
{
	if (!shape)
		return nullptr;

	BeginUpdateShape();
	PxTransform trans = shape->getLocalPose();
	float _radius = CalculateRadius();
	float3 rad_rot = float3(rotation.x, rotation.y, rotation.z) * DEGTORAD;
	trans.p = F3_TO_PXVEC3(center.Mul(physics->scale));
	trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rot.x, rad_rot.y, rad_rot.z));
	shape->setLocalPose(trans);
	shape->setGeometry(PxSphereGeometry(_radius));
	EndUpdateShape();

	return shape;
}

void ComponentSphereCollider::ScaleChanged()
{
	RecreateSphereShape();
}

void ComponentSphereCollider::DrawSpecificInspector()
{
	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &radius, 0.01f, 0.01f, FLT_MAX)) { RecreateSphereShape(); }
}

void ComponentSphereCollider::Reset()
{
	ComponentCollider::Reset();
	InitializeRadius();
	RecreateSphereShape();
}

void ComponentSphereCollider::SetRadius(float radius)
{
	this->radius = radius;
	RecreateSphereShape();
}

void ComponentSphereCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
}

void ComponentSphereCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	radius = (to_load->GetNumber("Radius"));
	RecreateSphereShape();
}

void ComponentSphereCollider::Clone(Component* clone)
{
	ComponentSphereCollider* sphere_clone = (ComponentSphereCollider*)clone;
	sphere_clone->radius = radius;
	sphere_clone->center = center;
	sphere_clone->rotation = rotation;
	sphere_clone->RecreateSphereShape();
}