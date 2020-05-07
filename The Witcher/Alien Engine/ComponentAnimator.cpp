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
#include "Time.h"
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
	{
		animator_controller->FreeMemory();
		delete animator_controller;
		animator_controller = nullptr;
		source_animator_controller->DecreaseReferences();
	}
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

void ComponentAnimator::PlayState(const char* name)
{
	if (animator_controller)
		animator_controller->Play(name);
}

void ComponentAnimator::SetStateSpeed(const char* name, float value)
{
	OPTICK_EVENT();

	std::vector<State*> states = animator_controller->GetStates();
	for (auto it = states.begin(); it != states.end(); ++it)
	{
		if (strcmp((*it)->GetName().c_str(), name) == 0) {
			(*it)->SetSpeed(value);
			break;
		}
	}
}

bool ComponentAnimator::IsPlaying(const char* name)
{
	OPTICK_EVENT();

	bool ret = false;

	if (strcmp(animator_controller->GetCurrentNode()->GetName().c_str(), name) == 0)
		ret = true;

	return ret;
}

void ComponentAnimator::IncreaseAllStateSpeeds(float value)
{
	OPTICK_EVENT();

	std::vector<State*> states = animator_controller->GetStates();
	for (auto it = states.begin(); it != states.end(); ++it)
	{
		(*it)->SetSpeed((*it)->GetSpeed() * value);
	}
}

void ComponentAnimator::DecreaseAllStateSpeeds(float value)
{
	OPTICK_EVENT();

	std::vector<State*> states = animator_controller->GetStates();
	for (auto it = states.begin(); it != states.end(); ++it)
	{
		(*it)->SetSpeed((*it)->GetSpeed() / value);
	}
}

void ComponentAnimator::UpdateAnimation(GameObject* go_to_update)
{
	OPTICK_EVENT();

	float3 position, scale;
	Quat rotation;

	if (animator_controller->GetTransform(go_to_update->GetName(), position, rotation, scale))
	{
		go_to_update->transform->SetLocalTransform(position, rotation, scale);
	}

	for (int i = 0; i < go_to_update->GetChildren().size(); i++)
	{
		UpdateAnimation(go_to_update->GetChildren()[i]);
	}
}

void ComponentAnimator::OnPlay()
{
	OPTICK_EVENT();

	if (source_animator_controller)
	{
		source_animator_controller->SaveAsset(source_animator_controller->GetID());
		animator_controller = new ResourceAnimatorController(source_animator_controller);
		animator_controller->mycomponent = this;
		source_animator_controller->DecreaseReferences();
		animator_controller->Play();
	}
}

ResourceAnimatorController* ComponentAnimator::GetResourceAnimatorController()
{
	return source_animator_controller;
}

ResourceAnimatorController* ComponentAnimator::GetCurrentAnimatorController()
{
	return animator_controller;
}

void ComponentAnimator::SetAnimatorController(ResourceAnimatorController* controller)
{
	OPTICK_EVENT();

	if (source_animator_controller) {
		source_animator_controller->DecreaseReferences();	}

	source_animator_controller = controller;
	source_animator_controller->IncreaseReferences();
}

void ComponentAnimator::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetString("ControllerID", source_animator_controller ? std::to_string(source_animator_controller->GetID()).data() : std::to_string(0).data());
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentAnimator::LoadComponent(JSONArraypack* to_load)
{
	OPTICK_EVENT();

	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	u64 controller_ID = std::stoull(to_load->GetString("ControllerID"));
	if (controller_ID != 0)
	{
		ResourceAnimatorController* anim_ctrl = (ResourceAnimatorController*)App->resources->GetResourceWithID(controller_ID);
		if (anim_ctrl != nullptr)
			SetAnimatorController(anim_ctrl);
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

void ComponentAnimator::SetCurrentStateSpeed(float speed)
{
	animator_controller->GetCurrentNode()->SetSpeed(speed);
}

float ComponentAnimator::GetCurrentStateDuration()
{
	return animator_controller->GetCurrentNode()->GetClip()->GetDuration() / animator_controller->GetCurrentNode()->GetSpeed();
}

float ComponentAnimator::GetCurrentStateSpeed()
{
	return animator_controller->GetCurrentNode()->GetSpeed();
}

float ComponentAnimator::GetCurrentStateTime()
{
	return animator_controller->GetCurrentNode()->time;
}

uint ComponentAnimator::GetCurrentAnimTPS()
{
	return animator_controller->GetCurrentNode()->GetClip()->ticks_per_second;
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
		
		ImGui::Button(source_animator_controller ? source_animator_controller->name.data():"No Controller Assigned", { ImGui::GetWindowWidth() * 0.55F , 0 });
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