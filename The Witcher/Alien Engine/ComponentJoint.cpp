#include "ComponentJoint.h"
#include "ComponentPhysics.h"
#include "ComponentRigidBody.h"
#include "GameObject.h"

#include "Application.h"
#include "ModuleResources.h"
ComponentJoint::ComponentJoint(GameObject* go) : ComponentBasePhysic(go)
{
	name = "Joint";
}

ComponentJoint::~ComponentJoint()
{
	DestroyJoint();
}

void ComponentJoint::SetConnectedBody(RigidBody* body)
{
	connected_body = body;

	if (joint)
		joint->setActors(physics->actor, (body) ? body->physics->actor : nullptr);
}

void ComponentJoint::SetAnchor(float3 _anchor)
{
	anchor = _anchor;
	if (joint)
		joint->setLocalPose(PxJointActorIndex::eACTOR0, PxTransform(F3_TO_PXVEC3(anchor)));
}

void ComponentJoint::CreateJoint()
{
	DestroyJoint();

	// Create Joint ------------------------

	joint = CreateSpecificJoint();

	// Setup Joint -------------------------

	SetConnectedBody(connected_body);
	SetAnchor(anchor);
	SetupSpecificJoint();
}

void ComponentJoint::DestroyJoint()
{
	if (joint)
	{
		joint->release();
		joint = nullptr;
	}
}

void ComponentJoint::DrawScene()
{
}

bool ComponentJoint::DrawInspector()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(name.c_str(), &not_destroy))
	{
		if (physics->rigid_body == nullptr)
		{
			ImGui::Spacing();
			ImGui::BeginChildFrame(1000, ImVec2(0,30));
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "  RigidBody is needed");
			ImGui::EndChildFrame();
		}
		
		ImGui::Spacing();

		ImGui::Title("Connected Body"); 
		if (ImGui::Button((connected_body) ? connected_body->go->name : "None (Rigid Body)", ImVec2(250, 0)))
		{
			SetConnectedBody(nullptr);
		}

		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
				GameObject* obj = *(GameObject**)payload->Data;
				if (obj != nullptr) {
					ComponentRigidBody* rb = obj->GetComponent<ComponentRigidBody>();
					if (rb != nullptr && rb->go != go)
					{
						SetConnectedBody(rb);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		float3 c_anchor = anchor;
		ImGui::Title("Anchor"); if (ImGui::DragFloat3("##anchor", c_anchor.ptr(), 0.05f)) { SetAnchor(c_anchor); }
		DrawInspectorJoint();
		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentJoint::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type) {
	case AlienEventType::RIGIDBODY_ADDED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		if (object->go == go)
		{
			CreateJoint();
		}
		break; }
	case AlienEventType::RIGIDBODY_DELETED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;

		if (object->go == go)
		{
			DestroyJoint();
		}

		if (object == connected_body)
		{
			SetConnectedBody(nullptr);
		}

		break; }
	}
}

void ComponentJoint::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ConnectedID", std::to_string((connected_body) ? connected_body->go->ID : 0).data());
	to_save->SetFloat3("Anchor", anchor);


}

void ComponentJoint::LoadComponent(JSONArraypack* to_load)
{
	u64 connected_id = std::stoull(to_load->GetString("ConnectedID", "0"));
	SetAnchor(to_load->GetFloat3("Anchor"));

	if (connected_id != 0)
	{
		GameObject*  rb_go = App->objects->GetGameObjectByID(connected_id);
		if (rb_go) 
			SetConnectedBody(rb_go->GetComponent<ComponentRigidBody>());
	}
}
