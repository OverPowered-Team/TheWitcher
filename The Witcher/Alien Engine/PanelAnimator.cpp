#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ShortCutManager.h"
#include "Time.h"

#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Resource_.h"
#include "ResourceAnimation.h"

#include "PanelProject.h"
#include "PanelAnimator.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

#define CHECKBOX_SIZE 50

void PanelAnimator::DrawStates()
{
	OPTICK_EVENT();
	for (uint i = 0, count = current_animator->GetNumStates(); i < count; ++i)
	{
		// Start drawing nodes.

		if (current_animator->GetCurrentNode()) {
			if (current_animator->GetStates()[i] == current_animator->GetCurrentNode()) {
				ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, { 0, 255, 0, 255 });
			}
		}

		ax::NodeEditor::BeginNode(current_animator->GetStates()[i]->id);


		ImGui::Text(current_animator->GetStates()[i]->GetName().c_str());

		if (current_animator->GetStates()[i] == current_animator->GetDefaultNode()) {
			ImGui::Text("Entry node");
		}

		if (current_animator->GetStates()[i]->GetClip())
			ImGui::Text(current_animator->GetStates()[i]->GetClip()->name.c_str());
		else
			ImGui::Text("No clip selected");

		ax::NodeEditor::BeginPin(++pin_in_id, ax::NodeEditor::PinKind::Input);
		current_animator->GetStates()[i]->pin_in_id = pin_in_id;
		ImGui::Text("-> In");
		ax::NodeEditor::EndPin();
		ImGui::SameLine();
		ax::NodeEditor::BeginPin(++pin_out_id, ax::NodeEditor::PinKind::Output);
		current_animator->GetStates()[i]->pin_out_id = pin_out_id;
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();

		if (current_animator->GetCurrentNode()) {
			if (current_animator->GetStates()[i] == current_animator->GetCurrentNode()) {
				ax::NodeEditor::PopStyleColor();
			}
		}
	}
}

void PanelAnimator::HandleContextMenu()
{
	OPTICK_EVENT();
	ax::NodeEditor::Suspend();


	context_node_id = 0;
	ax::NodeEditor::PinId context_pin_id = 0;
	ax::NodeEditor::LinkId context_link_id = 0;

	if (ax::NodeEditor::ShowBackgroundContextMenu()) {
		ImGui::OpenPopup("States popup");
	}

	if (ax::NodeEditor::ShowNodeContextMenu(&context_node_id)) {
		context_node = current_animator->FindState((uint)context_node_id)->GetName();
		ImGui::OpenPopup("State popup");
	}

	if (ax::NodeEditor::ShowLinkContextMenu(&context_link_id)) {
		selected_link_index = (uint)context_link_id;
		selected_link_index = selected_link_index - 300 - 1;
		ImGui::OpenPopup("Link popup");
	}

	ax::NodeEditor::Resume();
}

void PanelAnimator::DrawTransitions()
{
	OPTICK_EVENT();
	ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 4.0f);
	for (uint i = 0; i < current_animator->GetNumTransitions(); i++)
	{
		State* source = current_animator->FindState(current_animator->GetTransitions()[i]->GetSource()->GetName());
		State* target = current_animator->FindState(current_animator->GetTransitions()[i]->GetTarget()->GetName());

		ax::NodeEditor::Link(++link_id, source->pin_out_id, target->pin_in_id);
	}
	ax::NodeEditor::PopStyleVar(1);
}

void PanelAnimator::ShowStatePopup() {
	if (ImGui::BeginPopup("State popup")) {

		ImGui::Separator();
		ImGui::Separator();

		std::string temp_str = current_animator->FindState(context_node)->GetName();
		if (ImGui::InputText("Name", &temp_str, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			current_animator->FindState(context_node)->SetName(temp_str);
			ImGui::CloseCurrentPopup();
		}

		ImGui::Separator();

		if (ImGui::Selectable("Set as entry node")) {
			current_animator->SetDefaultNode(current_animator->FindState(context_node));
		}

		ImGui::Separator();

		ImGui::Text("Clip list");

		ImGui::Separator();

		std::vector<Resource*> clips = App->resources->GetResourcesWithType(ResourceType::RESOURCE_ANIMATION);

		for (int i = 0; i < clips.size(); i++) {
			if (ImGui::Selectable(((ResourceAnimation*)clips[i])->name.c_str())) {
				current_animator->FindState(context_node)->SetClip((ResourceAnimation*)clips[i]);
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::Separator();

		float tmp_speed = current_animator->FindState(context_node)->GetSpeed();
		if (ImGui::InputFloat("Speed", &tmp_speed)) {
			current_animator->FindState(context_node)->SetSpeed(tmp_speed);
		}

		ImGui::Separator();

		if (ImGui::Selectable("Delete State"))
		{
			ax::NodeEditor::DeleteNode(ax::NodeEditor::NodeId((context_node_id)));
			current_animator->RemoveState(context_node);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void PanelAnimator::CreateState()
{
	std::string name = "NewState";

	current_animator->AddState(name, nullptr);
	uint node_id = current_animator->GetStates().back()->id;
	ax::NodeEditor::SetNodePosition(node_id, ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos()));
	current_animator->GetStates().back()->SetName(name.append(std::to_string(current_animator->GetStates().back()->id)));

	if (new_node_id != ax::NodeEditor::PinId::Invalid)
	{
		State* target_state = current_animator->GetStates().back();
		current_animator->AddTransition(source_state, target_state, 0.25f);
	}
}

void PanelAnimator::HandleDropLink()
{
	if (ax::NodeEditor::BeginCreate())
	{
		ax::NodeEditor::PinId start_pin_id = 0, end_pin_id = 0;

		if (ax::NodeEditor::QueryNewLink(&start_pin_id, &end_pin_id)) {

			if (start_pin_id && end_pin_id)
			{
				bool start_is_input, end_is_input;

				if ((uint)start_pin_id > 100 && (uint)start_pin_id < 200)
					start_is_input = true;
				else
					start_is_input = false;

				if ((uint)end_pin_id > 100 && (uint)end_pin_id < 200)
					end_is_input = true;
				else
					end_is_input = false;

				State* start_node = nullptr;
				start_node = current_animator->FindStateFromPinId(start_pin_id.Get());
				State* end_node = nullptr;
				end_node = current_animator->FindStateFromPinId(end_pin_id.Get());

				if (start_pin_id == end_pin_id)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else if (start_is_input && end_is_input)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else if (start_node == end_node)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else {
					if (ax::NodeEditor::AcceptNewItem(ImColor(0, 255, 0), 4.0f))
					{
						if (start_is_input)
							current_animator->AddTransition(end_node, start_node, 0.25f);
						else
							current_animator->AddTransition(start_node, end_node, 0.25f);
					}
				}

			}
		}

		if (ax::NodeEditor::QueryNewNode(&new_node_id))
		{
			bool pin_is_input;

			if ((uint)new_node_id > 100 && (uint)new_node_id < 200)
				pin_is_input = true;
			else
				pin_is_input = false;

			if (!pin_is_input)
			{
				if (ax::NodeEditor::AcceptNewItem())
				{
					source_state = current_animator->FindStateFromPinId((uint)new_node_id);
					ax::NodeEditor::Suspend();
					new_node_pos = ImGui::GetMousePos();
					ImGui::OpenPopup("States popup");
					ax::NodeEditor::Resume();
				}
				else new_node_id = ax::NodeEditor::PinId::Invalid;
			}
		}
	}

	ax::NodeEditor::EndCreate();
}

void PanelAnimator::ShowNewStatePopup()
{
	if (ImGui::BeginPopup("States popup")) {
		if (ImGui::Selectable("NewState")) {
			CreateState();
			new_node_id = ax::NodeEditor::PinId::Invalid;
		}
		ImGui::EndPopup();
	}
}

void PanelAnimator::ShowLinkPopup()
{
	if (ImGui::BeginPopup("Link popup")) {

		//----------Bools-----------------

		ImGui::Text("Bool Conditions");
		ImGui::Separator();

		for (int i = 0; i < current_animator->GetTransitions()[selected_link_index]->GetBoolConditions().size(); i++)
		{
			ImGui::Separator();

			ImGui::PushID(current_animator->GetBoolParameters()[current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->parameter_index].first.c_str());
			if (ImGui::Button(current_animator->GetBoolParameters()[current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->parameter_index].first.c_str())) {
				ImGui::OpenPopup("Select bool parameter");
			}

			if (ImGui::BeginPopup("Select bool parameter")) {
				for (int j = 0; j < current_animator->GetBoolParameters().size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetBoolParameters()[j].first.c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->SetParameter(j);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button(current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->comp_text.c_str())) {
				ImGui::OpenPopup("Select bool comparison");
			}

			if (ImGui::BeginPopup("Select bool comparison")) {
				for (int j = 0; j < current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->comp_texts.size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->comp_texts[j].c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->SetCompText(current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]->comp_texts[j]);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove")) {
				current_animator->GetTransitions()[selected_link_index]->RemoveBoolCondition(current_animator->GetTransitions()[selected_link_index]->GetBoolConditions()[i]);
			}

			ImGui::PopID();


		}

		//----------Floats-----------------

		ImGui::Text("Float Conditions");
		ImGui::Separator();

		for (int i = 0; i < current_animator->GetTransitions()[selected_link_index]->GetFloatConditions().size(); i++)
		{
			ImGui::Separator();

			ImGui::PushID(current_animator->GetFloatParameters()[current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->parameter_index].first.c_str());
			if (ImGui::Button(current_animator->GetFloatParameters()[current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->parameter_index].first.c_str())) {
				ImGui::OpenPopup("Select float parameter");
			}

			if (ImGui::BeginPopup("Select float parameter")) {
				for (int j = 0; j < current_animator->GetFloatParameters().size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetFloatParameters()[j].first.c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->SetParameter(j);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button(current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->comp_text.c_str())) {
				ImGui::OpenPopup("Select float comparison");
			}


			if (ImGui::BeginPopup("Select float comparison")) {
				for (int j = 0; j < current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->comp_texts.size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->comp_texts[j].c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->SetCompText(current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->comp_texts[j]);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			ImGui::InputFloat("", &current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]->comp);

			ImGui::SameLine();

			if (ImGui::Button("Remove")) {
				current_animator->GetTransitions()[selected_link_index]->RemoveFloatCondition(current_animator->GetTransitions()[selected_link_index]->GetFloatConditions()[i]);
			}

			ImGui::PopID();


		}

		//----------Ints-----------------

		ImGui::Text("Int Conditions");
		ImGui::Separator();

		for (int i = 0; i < current_animator->GetTransitions()[selected_link_index]->GetIntConditions().size(); i++)
		{
			ImGui::Separator();

			ImGui::PushID(current_animator->GetIntParameters()[current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->parameter_index].first.c_str());
			if (ImGui::Button(current_animator->GetIntParameters()[current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->parameter_index].first.c_str())) {
				ImGui::OpenPopup("Select Int parameter");
			}

			if (ImGui::BeginPopup("Select Int parameter")) {
				for (int j = 0; j < current_animator->GetIntParameters().size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetIntParameters()[j].first.c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->SetParameter(j);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button(current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->comp_text.c_str())) {
				ImGui::OpenPopup("Select Int comparison");
			}

			if (ImGui::BeginPopup("Select Int comparison")) {
				for (int j = 0; j < current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->comp_texts.size(); j++)
				{
					if (ImGui::Selectable(current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->comp_texts[j].c_str())) {
						current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->SetCompText(current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->comp_texts[j]);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			ImGui::InputInt("", &current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]->comp);

			ImGui::SameLine();

			if (ImGui::Button("Remove")) {
				current_animator->GetTransitions()[selected_link_index]->RemoveIntCondition(current_animator->GetTransitions()[selected_link_index]->GetIntConditions()[i]);
			}

			ImGui::PopID();
		}

		ImGui::Separator();

		if (ImGui::Button("Add bool Condition"))
		{
			if (current_animator->GetBoolParameters().size() > 0)
				current_animator->GetTransitions()[selected_link_index]->AddBoolCondition();
		}

		if (ImGui::Button("Add float Condition"))
		{
			if (current_animator->GetFloatParameters().size() > 0)
				current_animator->GetTransitions()[selected_link_index]->AddFloatCondition();
		}

		if (ImGui::Button("Add int Condition"))
		{
			if (current_animator->GetIntParameters().size() > 0)
				current_animator->GetTransitions()[selected_link_index]->AddIntCondition();
		}

		ImGui::Separator();

		float blend_v = (float)current_animator->GetTransitions()[selected_link_index]->GetBlend();

		if (ImGui::InputFloat("Blend value: ", &blend_v)) {
			current_animator->GetTransitions()[selected_link_index]->SetBlend(blend_v);
		}

		ImGui::Separator();

		bool end_v = current_animator->GetTransitions()[selected_link_index]->GetEnd();

		if (ImGui::Checkbox("End: ", &end_v)) {
			current_animator->GetTransitions()[selected_link_index]->SetEnd(end_v);
		}

		ImGui::Separator();

		if (ImGui::Selectable("Delete Transition")) {
			current_animator->RemoveTransition(current_animator->GetTransitions()[selected_link_index]->GetSource()->GetName(), current_animator->GetTransitions()[selected_link_index]->GetTarget()->GetName());
		}


		ImGui::EndPopup();
	}
}

void PanelAnimator::Start()
{

}

bool PanelAnimator::IsInside(const float2& pos) const
{
	AABB2D box(float2(screen_pos.x, screen_pos.y), float2(screen_pos.x + w, screen_pos.y + h));
	return math::Contains(box, float3(pos.x, pos.y, 0));
}

void PanelAnimator::DrawParameterList()
{
	OPTICK_EVENT();
	if (current_animator->GetBoolParameters().size() > 0) {
		for (int i = 0; i < current_animator->GetBoolParameters().size(); i++) {

			//name
			ImGui::PushID(&current_animator->GetBoolParameters()[i].first);
			std::string temp = current_animator->GetBoolParameters()[i].first;
			if (ImGui::InputText("##inputtextbool", &temp)) {
				current_animator->SetBoolParametersName(i, temp);
			}

			ImGui::SameLine();

			//value
			bool temp_value = current_animator->GetBoolParameters()[i].second;
			if (ImGui::Checkbox("##checkbox", &temp_value)) {
				current_animator->SetBoolParametersValue(i, temp_value);
			}

			if (ImGui::Button("Remove")) {
				current_animator->RemoveBoolParameter(current_animator->GetBoolParameters()[i].first);
			}

			ImGui::PopID();
			ImGui::Separator();
		}
	}

	if (current_animator->GetFloatParameters().size() > 0) {
		for (int i = 0; i < current_animator->GetFloatParameters().size(); i++) {

			//name
			ImGui::PushID(&current_animator->GetFloatParameters()[i].first);
			std::string temp = current_animator->GetFloatParameters()[i].first;
			if (ImGui::InputText("##inputtextfloat", &temp)) {
				current_animator->SetFloatParametersName(i, temp);
			}

			ImGui::SameLine();

			//value
			float temp_value = current_animator->GetFloatParameters()[i].second;
			if (ImGui::InputFloat("##inputfloat", &temp_value)) {
				current_animator->SetFloatParametersValue(i, temp_value);
			}

			if (ImGui::Button("Remove")) {
				current_animator->RemoveFloatParameter(current_animator->GetFloatParameters()[i].first);
			}

			ImGui::PopID();
			ImGui::Separator();
		}
	}

	if (current_animator->GetIntParameters().size() > 0) {
		for (int i = 0; i < current_animator->GetIntParameters().size(); i++) {
			//name
			ImGui::PushID(&current_animator->GetIntParameters()[i].first);
			std::string temp = current_animator->GetIntParameters()[i].first;
			if (ImGui::InputText("##inputtextint", &temp)) {
				current_animator->SetIntParametersName(i, temp);
			}

			ImGui::SameLine();

			//value
			int temp_value = current_animator->GetIntParameters()[i].second;;
			if (ImGui::InputInt("##inputint", &temp_value)) {
				current_animator->SetIntParametersValue(i, temp_value);
			}

			if (ImGui::Button("Remove")) {
				current_animator->RemoveIntParameter(current_animator->GetIntParameters()[i].first);
			}

			ImGui::PopID();
			ImGui::Separator();
		}
	}
}

void PanelAnimator::OnAssetSelect()
{
	if (App->ui->panel_project->current_active_file->type == FileDropType::ANIM_CONTROLLER)
	{
		std::string asset_path = App->ui->panel_project->current_active_file->path + App->ui->panel_project->current_active_file->name;
		std::string alien_path = App->file_system->GetPathWithoutExtension(asset_path) + "_meta.alien";
		u64 resource_id = App->resources->GetIDFromAlienPath(alien_path.data());
		ResourceAnimatorController* anim_ctrl = (ResourceAnimatorController*)App->resources->GetResourceWithID(resource_id);
		if (current_animator != anim_ctrl)
		{
			SetCurrentResourceAnimatorController(anim_ctrl);
		}
	}
}

void PanelAnimator::OnAssetDelete()
{
	if (App->ui->panel_project->current_active_file->type == FileDropType::ANIM_CONTROLLER)
	{
		std::string asset_path = App->ui->panel_project->current_active_file->path + App->ui->panel_project->current_active_file->name;
		std::string alien_path = App->file_system->GetPathWithoutExtension(asset_path) + "_meta.alien";
		u64 resource_id = App->resources->GetIDFromAlienPath(alien_path.data());
		ResourceAnimatorController* anim_ctrl = (ResourceAnimatorController*)App->resources->GetResourceWithID(resource_id);
		if (current_animator)
			current_animator->DecreaseReferences();
		current_animator = nullptr;
	}
}

void PanelAnimator::OnObjectSelect()
{
	if (App->objects->GetSelectedObjects().size() == 1)
	{
		if (App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::ANIMATOR)) {
			ComponentAnimator* c_anim = (ComponentAnimator*)App->objects->GetSelectedObjects().back()->GetComponent(ComponentType::ANIMATOR);
			ResourceAnimatorController* anim_ctrl = nullptr;
			if (Time::GetGameState() == Time::GameState::PLAY)
				current_animator = c_anim->GetCurrentAnimatorController();
			else {
				anim_ctrl = c_anim->GetResourceAnimatorController();

				if (current_animator != anim_ctrl)
				{
					SetCurrentResourceAnimatorController(anim_ctrl);
				}
			}

		}else
			current_animator = nullptr;
	}
	else if (App->objects->GetSelectedObjects().size() > 1) {
		for each (GameObject * go in App->objects->GetSelectedObjects())
		{
			if (go->HasComponent(ComponentType::ANIMATOR)) {
				ComponentAnimator* c_anim = (ComponentAnimator*)go->GetComponent(ComponentType::ANIMATOR);

				ResourceAnimatorController* anim_ctrl = nullptr;
				if (Time::GetGameState() == Time::GameState::PLAY)
					current_animator = c_anim->GetCurrentAnimatorController();
				else {
					anim_ctrl = c_anim->GetResourceAnimatorController();

					if (current_animator != anim_ctrl)
					{
						SetCurrentResourceAnimatorController(anim_ctrl);
					}
				}
				break;
			}else
				current_animator = nullptr;
		}
	}
}

void PanelAnimator::OnObjectDelete()
{
	//TODO
}

void PanelAnimator::OnPlay()
{
	OnObjectSelect();
}

void PanelAnimator::OnStop()
{
	if (App->objects->GetSelectedObjects().size() == 1)
	{
		if (App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::ANIMATOR)) {
			ComponentAnimator* c_anim = (ComponentAnimator*)App->objects->GetSelectedObjects().back()->GetComponent(ComponentType::ANIMATOR);
			ResourceAnimatorController* anim_ctrl = nullptr;

			current_animator = c_anim->GetResourceAnimatorController();
		}
		else
			current_animator = nullptr;
	}
	else if (App->objects->GetSelectedObjects().size() > 1) {
		for each (GameObject * go in App->objects->GetSelectedObjects())
		{
			if (go->HasComponent(ComponentType::ANIMATOR)) {
				ComponentAnimator* c_anim = (ComponentAnimator*)go->GetComponent(ComponentType::ANIMATOR);
				ResourceAnimatorController* anim_ctrl = nullptr;

				current_animator = c_anim->GetResourceAnimatorController();
				break;
			}
			else
				current_animator = nullptr;
		}
	}
}

void PanelAnimator::SetCurrentResourceAnimatorController(ResourceAnimatorController* animator)
{
	if (animator == nullptr)
		return;

	if (current_animator)
	{
		current_animator->SaveAsset(current_animator->GetID());
		current_animator->DecreaseReferences();
	}

	current_animator = animator;
	current_animator->IncreaseReferences();
}

PanelAnimator::PanelAnimator(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Animator", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
}

PanelAnimator::~PanelAnimator()
{
	if (current_animator)
		current_animator->SaveAsset(current_animator->GetID());
}

bool PanelAnimator::FillInfo()
{
	return false;
}

void PanelAnimator::PanelLogic()
{
	OPTICK_EVENT();
	ImGuiWindowFlags aboutFlags = 0;
	aboutFlags |= ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::Begin("Animator", &enabled, aboutFlags);

	if (FillInfo())
	{

	}

	if (current_animator) {
		ax::NodeEditor::SetCurrentEditor(current_animator->GetEditorContext());

		ax::NodeEditor::Begin("Animator");

		unique_id = 0;
		pin_in_id = 100;
		pin_out_id = 200;
		link_id = 300;

		// Start drawing nodes.
		DrawStates();
		DrawTransitions();
		HandleDropLink();

		HandleContextMenu();

		ax::NodeEditor::Suspend();
		ShowNewStatePopup();
		ShowStatePopup();
		ShowLinkPopup();
		ax::NodeEditor::Resume();

		ax::NodeEditor::End();
		ax::NodeEditor::SetCurrentEditor(nullptr);

		ImGui::SetCursorPos({ 4, 24 });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));

		ImGui::BeginChild("Parameters", { 200, 350 }, true);
		ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen);

		DrawParameterList();

		ImGui::Separator();

		if (ImGui::Button("Add Parameter")) {
			ImGui::OpenPopup("Add parameter...");
		}

		if (ImGui::BeginPopup("Add parameter..."))
		{
			if (ImGui::Selectable("bool")) {
				current_animator->AddBoolParameter();
			}

			if (ImGui::Selectable("float")) {
				current_animator->AddFloatParameter();
			}

			if (ImGui::Selectable("int")) {
				current_animator->AddIntParameter();
			}

			ImGui::EndPopup();
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	ImGui::End();
}
