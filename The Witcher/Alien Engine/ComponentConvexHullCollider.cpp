//#include "Application.h"
//
//#include "C_ConvexHullCollider.h"
//#include "C_Mesh.h"
//#include "R_Mesh.h"
//#include "GameObject.h"
//
//ComponentConvexHullCollider::ComponentConvexHullCollider(GameObject* go) : ComponentCollider(go)
//{
//	name.assign("Convex Hull Collider");
//	type = ComponentType::CONVEX_HULL_COLLIDER;
//	size = float3::one;
//}
//
//ComponentConvexHullCollider::~ComponentConvexHullCollider()
//{
//	delete hull;
//}
//
//void ComponentConvexHullCollider::CreateShape(C_Mesh* mesh)
//{
//	if (is_loaded == false)
//	{
//		center = float3::zero;
//	}
//
//	if (mesh == nullptr)
//		return;
//
//	R_Mesh* r_mesh = dynamic_cast<R_Mesh*>(App->resources->Get(mesh->GetResource()));
//
//	if (r_mesh == nullptr)
//		return;
//
//	btConvexHullShape new_shape(r_mesh->vertices, r_mesh->buffers_size[0], sizeof(float) * 3);
//	hull = new btShapeHull(&new_shape);
//	hull->buildHull(new_shape.getMargin());
//
//	btConvexHullShape* pConvexHull = new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices());
//	pConvexHull->setUserPointer(hull);
//	
//	shape = pConvexHull;
//
//}
//
//bool ComponentConvexHullCollider::Render()
//{
//	if (aux_body == nullptr || shape == nullptr) return true;
//
//	if (App->scene->selected_go == linked_go && App->scene->editor_mode && hull)
//	{
//		App->physics->RenderConvexCollider(this);
//	}
//
//	return true;
//}
//
//void ComponentConvexHullCollider::AdjustShapeToAABB()
//{
//	scaled_center = center.Mul(linked_go->transform->scale);
//	float3 scaled_size = size.Mul(linked_go->transform->scale.Abs());
//	scaled_size = CheckInvalidCollider(scaled_size);
//	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));
//}
//
//void ComponentConvexHullCollider::SaveCollider(Config& config)
//{
//
//}
//
//void ComponentConvexHullCollider::LoadCollider(Config& config)
//{
//}
//
//float3 ComponentConvexHullCollider::CheckInvalidCollider(float3 size)
//{
//	return size.Max(float3(0.01, 0.01, 0.01));
//}
//
//void ComponentConvexHullCollider::DrawPanelColliderInfo()
//{
//	//ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
//}
//
