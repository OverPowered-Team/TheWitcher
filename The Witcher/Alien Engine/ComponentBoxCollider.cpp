#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentBoxCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"

#include "Optick/include/optick.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;

	size = GetLocalMeshAabbSize();
	float3 dimensions = CalculateSize();
	shape = App->physx->CreateShape(PxBoxGeometry(dimensions.x, dimensions.y, dimensions.z), *material);
	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
	InitCollider();
}

void ComponentBoxCollider::ScaleChanged()
{
	ReCreateBoxShape();
}

PxShape* ComponentBoxCollider::ReCreateBoxShape()
{
	OPTICK_EVENT();

	if (!shape)
		return nullptr;

	BeginUpdateShape();
	PxTransform trans = shape->getLocalPose();
	float3 dimensions = CalculateSize();
	float3 rad_rot = float3(rotation.x, rotation.y, rotation.z) * DEGTORAD;
	trans.p = F3_TO_PXVEC3(center.Mul(transform->GetGlobalScale()));
	trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rot.x, rad_rot.y, rad_rot.z));
	shape->setLocalPose(trans);
	shape->setGeometry(PxBoxGeometry(dimensions.x, dimensions.y, dimensions.z));
	EndUpdateShape();

	return shape;
}

const float3 ComponentBoxCollider::CalculateSize()
{
	return size.Mul(transform->GetGlobalScale()) * 0.5f;
}

void ComponentBoxCollider::DrawSpecificInspector()
{
	ImGui::Title("Size", 1);
	if (ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX)) {
		ReCreateBoxShape();
	};
}

void ComponentBoxCollider::Clone(Component* clone)
{
	ComponentBoxCollider* box_clone = (ComponentBoxCollider*)clone;
	box_clone->center = center;
	box_clone->size = size;
	box_clone->rotation = rotation;
	box_clone->ReCreateBoxShape();
}

void ComponentBoxCollider::Reset()
{
	ComponentCollider::Reset();
	size = GetLocalMeshAabbSize();
	ReCreateBoxShape();
}

void ComponentBoxCollider::SaveComponent(JSONArraypack* to_save)
{
	OPTICK_EVENT();

	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);
}

void ComponentBoxCollider::LoadComponent(JSONArraypack* to_load)
{
	OPTICK_EVENT();

	ComponentCollider::LoadComponent(to_load);
	size = to_load->GetFloat3("Size");
	ReCreateBoxShape();
}

// * --------- ACCESS THROUGH SCRIPTING ----------* //

void ComponentBoxCollider::SetSize(float3 size)
{
	OPTICK_EVENT();
	this->size = size;
	ReCreateBoxShape();
}