#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentConvexHullCollider.h"
#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "mmgr/mmgr.h"

ComponentConvexHullCollider::ComponentConvexHullCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Convex Hull Collider");
	type = ComponentType::CONVEX_HULL_COLLIDER;

	size = float3::one();
	final_size = float3::one();

	// More useful if its called externaly
	Init();
}


void ComponentConvexHullCollider::Clone(Component* clone)
{
}

void ComponentConvexHullCollider::Reset()
{
}

void ComponentConvexHullCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);

}

void ComponentConvexHullCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetSize(to_load->GetFloat3("Size"));
}

void ComponentConvexHullCollider::CreateDefaultShape()
{
	mesh = game_object_attached->GetComponent<ComponentMesh>();

	size = float3::one();
	center = float3::zero();

	UpdateShape();
}

void ComponentConvexHullCollider::UpdateShape()
{
	if (shape != nullptr)
	{
		delete shape;
	}
	if (hull != nullptr)
	{
		delete hull;
	}

	if (mesh != nullptr)
	{
		ResourceMesh* resource_mesh = mesh->mesh;

		btConvexHullShape new_shape(resource_mesh->vertex, resource_mesh->num_vertex, sizeof(float) * 3);
		hull = new btShapeHull(&new_shape);
		hull->buildHull(new_shape.getMargin());
	}
	else
	{
		float cube_vertices[24] = {  // Coordinates for the vertices of a cube.
			0.5f,0.5f,0.5f,   0.5f,0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f,0.5f,
			-0.5f,0.5f,0.5f,  -0.5f,0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,0.5f };

		btConvexHullShape new_shape(cube_vertices, 24, sizeof(float) * 3);
		hull = new btShapeHull(&new_shape);
		hull->buildHull(new_shape.getMargin());
	}

	shape = new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices());
	shape->setUserPointer(hull);

	SetSize(size);

	if (aux_body)
		aux_body->setCollisionShape(shape);
	if (detector)
		detector->setCollisionShape(shape);

	if (rigid_body != nullptr)  rigid_body->UpdateCollider();
}

void ComponentConvexHullCollider::SetCenter(float3 value)
{
	center = value;
	final_center = center.Mul(final_size);
}

void ComponentConvexHullCollider::SetSize(float3 size)
{
	this->size = size;

	final_size = this->size.Mul(transform->GetGlobalScale());
	final_center = center.Mul(transform->GetGlobalScale());

	final_size = this->size.Mul(transform->GetGlobalScale().Abs());
	final_size = CheckInvalidCollider(final_size);
	shape->setLocalScaling(btVector3(final_size.x, final_size.y, final_size.z));
}

void ComponentConvexHullCollider::SetScale(const float3 scale)
{
	SetSize(size);
}

void ComponentConvexHullCollider::DrawScene()
{
	if (game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		App->physics->DrawConvexCollider(this);
	}
}

void ComponentConvexHullCollider::DrawSpecificInspector()
{
	float3 current_size = size;

	ImGui::Title("Size", 1);	if (ImGui::DragFloat3("##size", current_size.ptr(), 0.1f, 0.01f, FLT_MAX)) { SetSize(current_size); }
}

float3 ComponentConvexHullCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}