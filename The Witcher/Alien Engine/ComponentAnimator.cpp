#include "ComponentAnimator.h"
#include "Application.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "FileNode.h"
#include "ComponentTransform.h"
#include "NodeEditor/Include/imgui_node_editor.h"
#include "ResourceAnimation.h"
#include "ResourceAnimatorController.h"
#include "ComponentAnimator.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ComponentAnimator::ComponentAnimator(GameObject* gameobject) : Component(gameobject)
{
	type = ComponentType::ANIMATOR;
	animator_controller = nullptr;
}

ComponentAnimator::~ComponentAnimator()
{
	if (animator_controller)
		animator_controller->DecreaseReferences();
}

void ComponentAnimator::Update()
{
	OPTICK_EVENT();
	if (!animator_controller)
		return;

	animator_controller->Update();

	if (game_object_attached && Time::state == Time::GameState::PLAY)
		UpdateAnimation(game_object_attached);
}

void ComponentAnimator::PlayState(std::string name)
{
	if (animator_controller)
		animator_controller->Play(name);
}

void ComponentAnimator::UpdateAnimation(GameObject* go_to_update)
{
	float3 position, scale;
	Quat rotation;

	if (animator_controller->GetTransform(go_to_update->GetName(), position, rotation, scale))
	{
		go_to_update->transform->SetLocalPosition(position);
		go_to_update->transform->SetLocalRotation(rotation);
		go_to_update->transform->SetLocalScale(scale);
	}

	for (int i = 0; i < go_to_update->GetChildren().size(); i++)
	{
		UpdateAnimation(go_to_update->GetChildren()[i]);
	}
}

void ComponentAnimator::OnPlay()
{
	if (animator_controller)
	{
		animator_controller->SaveAsset(animator_controller->GetID());
		animator_controller->Play();
	}
}

ResourceAnimatorController* ComponentAnimator::GetResourceAnimatorController()
{
	return animator_controller;
}

void ComponentAnimator::SetAnimatorController(ResourceAnimatorController* controller)
{
	if (animator_controller) {
		animator_controller->DecreaseReferences();
		animator_controller->times_attached--;
	}

	animator_controller = controller;
	animator_controller->times_attached++;
	animator_controller->IncreaseReferences();
}

void ComponentAnimator::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetString("ControllerID", animator_controller ? std::to_string(animator_controller->GetID()) : std::to_string(0));
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentAnimator::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	u64 controller_ID = std::stoull(to_load->GetString("ControllerID"));
	if (controller_ID != 0)
	{
		animator_controller = (ResourceAnimatorController*)App->resources->GetResourceWithID(controller_ID);
		if (animator_controller != nullptr)
			animator_controller->IncreaseReferences();
	}
}

void ComponentAnimator::SetBool(const char* parameter_name, bool parameter_value)
{
	animator_controller->SetBool(parameter_name, parameter_value);
}

void ComponentAnimator::SetFloat(const char* parameter_name, float parameter_value)
{
	animator_controller->SetFloat(parameter_name, parameter_value);
}

void ComponentAnimator::SetInt(const char* parameter_name, int parameter_value)
{
	animator_controller->SetInt(parameter_name, parameter_value);
}

bool ComponentAnimator::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Animator", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen)){
		ImGui::Text("Controller");
		ImGui::SameLine();
		
		ImGui::Button(animator_controller ? animator_controller->name.data():"No Controller Assigned", { ImGui::GetWindowWidth() * 0.55F , 0 });
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::ANIM_CONTROLLER) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceAnimatorController* controller = (ResourceAnimatorController*)App->resources->GetResourceWithID(ID);
						if (controller != nullptr) {
							SetAnimatorController(controller);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	return true;
}