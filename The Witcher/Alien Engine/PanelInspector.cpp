#include "PanelInspector.h"
#include "ModuleObjects.h"
#include "ModuleRenderer3D.h"
#include "ModuleUI.h"
#include "PanelProject.h"
#include "ResourceScript.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentCurve.h"
#include "ComponentLightPoint.h"
#include "ComponentAnimator.h"
#include "ModuleResources.h"

#include "ResourceAnimation.h"
#include "ResourceModel.h"
#include "ResourceBone.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "imgui/imgui_internal.h"

#include "ComponentAudioListener.h"
#include "ComponentAudioEmitter.h"
#include "ComponentParticleSystem.h"
#include "ComponentSlider.h"
#include "ComponentCanvas.h"
#include "ComponentUI.h"
#include "ComponentImage.h"
#include "ComponentBar.h"
#include "ComponentText.h"
#include "ComponentButton.h"
#include "ComponentCheckbox.h"
#include "ComponentAnimatedImage.h"

#include "ReturnZ.h"
#include "Alien.h"
#include "ComponentScript.h"
#include "ShortCutManager.h"
#include "ModuleCamera3D.h"

#include "mmgr/mmgr.h"

#include "ComponentCollider.h"
#include "ComponentBoxCollider.h"
#include "ComponentSphereCollider.h"
#include "ComponentCapsuleCollider.h"
#include "ComponentConvexHullCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentCharacterController.h"

#include "Optick/include/optick.h"

#define DAE_FPS 30

PanelInspector::PanelInspector(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Inspector", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);

	components.push_back(std::pair<std::string, ComponentType>("AASelect Component", ComponentType::NONE)); // This name is for the sort in order to have it at the begin
	for (int i = 0; i < (int)ComponentType::MAX; i++) {
		if (i != (int)ComponentType::BONE && i != (int)ComponentType::MESH && i != (int)ComponentType::DEFORMABLE_MESH && i != (int)ComponentType::MAX && i != (int)ComponentType::UI) //Add the component types you don't want to show in combo
			components.push_back(
				std::pair<std::string, ComponentType>(
					Component::EnumToString((ComponentType)i), (ComponentType)i)
			);
	}

	//if there is an empty id in ComponentType we have to exclude if they have no name
	auto i = components.begin();
	while (i != components.end()) {
		if ((*i).first.compare("Not valid") == 0)
			i = components.erase(i);
		else
			++i;
	}

	std::sort(components.begin(), components.end()); // Sort components by name

	for (auto i = components.begin(); i != components.end(); i++) { // iterate and add the name and '\0' to the combo for imgui
		if ((*i).second == ComponentType::NONE)
			(*i).first.assign("Select Component"); // rename NONE id
		combo_select += (*i).first + '\0';
	}
}

PanelInspector::~PanelInspector()
{
}

void PanelInspector::PanelLogic()
{
	OPTICK_EVENT();
	ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	if (ImGui::IsWindowHovered())
		App->camera->is_scene_hovered = false;
	//selected gameobject/s
	if (App->objects->GetSelectedObjects().size() == 1)
	{
		static bool draw_add = true;
		GameObject* obj = App->objects->GetSelectedObjects().back();
		std::vector<Component*>::iterator item = obj->components.begin();
		for (; item != obj->components.end(); ++item)
		{
			if (*item != nullptr)
			{
				if ((*item)->DrawInspector()) {
					if (!(*item)->not_destroy && (*item)->serialize) {
						to_destroy = (*item);
						delete_panel = &(*item)->not_destroy;
						*delete_panel = !(*delete_panel);
					}
				}
				else {
					draw_add = false;
					break;
				}
			}
		}
		if (draw_add) {
			ButtonAddComponent();
		}
		else {
			draw_add = true;
		}

		DropScript();

	}
	else if (App->objects->GetSelectedObjects().size() > 1) 
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Spacing();
			ImGui::Text("Position  ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(1);

			std::list<GameObject*> selected = App->objects->GetSelectedObjects();
			auto item = selected.begin();
			float4x4 trans = float4x4::zero();
			bool some_static = false;

			for (; item != selected.end(); ++item) {
				if (*item != nullptr) {
					if ((*item)->is_static) {
						some_static = true;
					}
					if (trans.Equals(float4x4::zero())) {
						trans = (*item)->GetComponent<ComponentTransform>()->global_transformation;
					}
					else {
						trans = trans * (*item)->GetComponent<ComponentTransform>()->global_transformation;
					}

				}
			}
			float3 view_pos, view_scale, view_rot;
			Quat rot;
			trans.Decompose(view_pos, rot, view_scale);
			view_rot = rot.ToEulerXYZ();
			view_rot = RadToDeg(view_rot) / selected.size();
			view_pos /= selected.size();
			view_scale /= selected.size();
			float3 original_pos, original_rot, original_scale;
			original_pos = view_pos;
			original_rot = view_rot;
			original_scale = view_scale;

			bool need_refresh_pos = false;
			bool need_refresh_scale = false;
			bool need_refresh_rot = false;

			if (ImGui::DragFloat("X", &view_pos.x, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_pos = true;
			}

			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(2);
			if (ImGui::DragFloat("Y", &view_pos.y, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_pos = true;
			}

			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(3);
			if (ImGui::DragFloat("Z", &view_pos.z, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_pos = true;
			}

			ImGui::PopID();
			ImGui::Spacing();

			ImGui::Text("Rotation  ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(4);

			if (ImGui::DragFloat("X", &view_rot.x, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_rot = true;
			}

			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(5);
			if (ImGui::DragFloat("Y", &view_rot.y, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_rot = true;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(6);
			if (ImGui::DragFloat("Z", &view_rot.z, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_rot = true;
			}
			ImGui::PopID();
			ImGui::Spacing();

			ImGui::Text("Scale     ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(7);
			if (ImGui::DragFloat("X", &view_scale.x, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_scale = true;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(8);
			if (ImGui::DragFloat("Y", &view_scale.y, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_scale = true;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(70);
			ImGui::PushID(9);
			if (ImGui::DragFloat("Z", &view_scale.z, 0.5F, 0, 0, "%.3f", 1, some_static)) {
				need_refresh_scale = true;
			}
			ImGui::PopID();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (need_refresh_pos) {
				item = selected.begin();
				for (; item != selected.end(); ++item) {
					if (*item != nullptr) {
						(*item)->GetComponent<ComponentTransform>()->AddPosition(view_pos - original_pos);
					}
				}
			}
			else if (need_refresh_rot) {
				item = selected.begin();
				for (; item != selected.end(); ++item) {
					if (*item != nullptr) {
						(*item)->GetComponent<ComponentTransform>()->AddRotation(view_rot - original_rot);
					}
				}
			}
			else if (need_refresh_scale) {
				item = selected.begin();
				for (; item != selected.end(); ++item) {
					if (*item != nullptr) {
						(*item)->GetComponent<ComponentTransform>()->AddScale(view_scale - original_scale);
					}
				}
			}
		}
		ButtonAddComponent();
	}
	else if (App->ui->panel_project->selected_resource != nullptr)
	{
		Resource* selected_file = App->ui->panel_project->selected_resource;
		if (selected_file->GetType() == ResourceType::RESOURCE_MODEL)
		{
			ShowModelImportSettings((ResourceModel*)selected_file);
		}
		else if (selected_file->GetType() == ResourceType::RESOURCE_MATERIAL)
		{
			static_cast<ResourceMaterial*>(selected_file)->DisplayMaterialOnInspector();
		}
	}

	ImGui::End();

	DeleteComponentPopup();
}

void PanelInspector::DeleteComponentPopup()
{
	if (delete_panel != nullptr && *delete_panel) {
		ImGui::OpenPopup("Do you want to delete it?");
		ImGui::SetNextWindowSize({ 200,60 });
		if (ImGui::BeginPopupModal("Do you want to delete it?", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Spacing();
			ImGui::NewLine();
			ImGui::SameLine(40);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.5F,0,0,1 });
			if (ImGui::Button("Delete")) {
				*delete_panel = !(*delete_panel);
				delete_panel = nullptr;
				App->objects->need_to_delete_objects = true;
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::DELETE_COMPONENT, to_destroy);
				to_destroy = nullptr;
			}
			ImGui::PopStyleColor();
			ImGui::SameLine(100);
			if (ImGui::Button("Cancel")) {
				delete_panel = nullptr;
			}
			ImGui::EndPopup();
		}
	}
}

void PanelInspector::ButtonAddComponent()
{
	ImGui::Spacing();

	if (components[component].second == ComponentType::SCRIPT) {
		if (ImGui::BeginCombo("##Scriptss", std::get<0>(script_info)))
		{
			bool sel = App->StringCmp("Return To Components", std::get<0>(script_info));
			ImGui::Selectable("Return To Components", sel);
			if (ImGui::IsItemClicked())
			{
				std::get<0>(script_info) = "Return To Components";
				component = 0;
			}

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_SCRIPT) {
					ResourceScript* script = (ResourceScript*)(*item);
					if (!script->data_structures.empty()) {
						for (uint i = 0; i < script->data_structures.size(); ++i) {
							bool is_selected = (std::get<0>(script_info) == script->data_structures[i].first.data());
							if (ImGui::Selectable(script->data_structures[i].first.data(), is_selected))
							{
								std::get<0>(script_info) = script->data_structures[i].first.data();
								std::get<1>(script_info) = script->data_structures[i].second;
								std::get<2>(script_info) = script->GetID();
							}
						}
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();

		if (ImGui::Button("Add Component")) {
			if (!App->StringCmp("Return To Components", std::get<0>(script_info))) {
				bool added = false;
				for (auto item = App->objects->GetSelectedObjects().begin(); item != App->objects->GetSelectedObjects().end(); ++item) {
					GameObject* obj = *item;
					bool exists = false;
					std::vector<ComponentScript*> scripts = obj->GetComponents<ComponentScript>();
					for (uint i = 0; i < scripts.size(); ++i) {
						if (App->StringCmp(scripts[i]->data_name.data(), std::get<0>(script_info))) {
							exists = true;
							break;
						}
					}
					if (!exists) {
						ComponentScript* comp_script = new ComponentScript(obj);
						comp_script->resourceID = std::get<2>(script_info);
						comp_script->LoadData(std::get<0>(script_info), std::get<1>(script_info));
						added = true;
						ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, (void*)comp_script);
						if (Time::IsInGameState() && comp_script->need_alien && comp_script->data_ptr != nullptr) {
							Alien* alien = (Alien*)comp_script->data_ptr;
							if (alien != nullptr) {
								alien->Awake();
								alien->Start();
							}
						}
						break;
					}
					else {
						LOG_ENGINE("This script is already attached!");
					}
				}
				if (added) {
					std::get<0>(script_info) = "Return To Components";
					component = 0;
				}
			}
			else {
				LOG_ENGINE("Select a script");
			}
		}
	}

	else {
		ImGui::Combo("##choose component", &component, combo_select.c_str());
		ImGui::SameLine();

		if (ImGui::Button("Add Component"))
		{
			bool added = true;
			for (auto item = App->objects->GetSelectedObjects().begin(); item != App->objects->GetSelectedObjects().end(); ++item) {
				Component* comp = nullptr;
				GameObject* selected = *item;
				switch (components[component].second)
				{

				case ComponentType::NONE: {
					LOG_ENGINE("Select a Component!");
					break; }

				case ComponentType::MESH: {

					if (!selected->HasComponent(ComponentType::MESH))
					{
						comp = new ComponentMesh(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }
				case ComponentType::MATERIAL: {

					if ((!selected->HasComponent(ComponentType::MATERIAL)) &&
						(selected->HasComponent(ComponentType::MESH) || selected->HasComponent(ComponentType::DEFORMABLE_MESH)))
					{
						comp = new ComponentMaterial(selected);
						selected->AddComponent(comp);
					}

					else if (selected->HasComponent(ComponentType::MATERIAL))
					{
						LOG_ENGINE("The selected object already has this component!");
					}

					else
						LOG_ENGINE("The object needs a mesh to have a material!");

					break; }

				case ComponentType::LIGHT_DIRECTIONAL: {

					if (!selected->HasComponent(ComponentType::LIGHT_DIRECTIONAL))
					{
						comp = new ComponentLightDirectional(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }
				case ComponentType::CURVE: {

					if (!selected->HasComponent(ComponentType::CURVE))
					{
						comp = new ComponentCurve(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }
				case ComponentType::LIGHT_SPOT: {

					if (!selected->HasComponent(ComponentType::LIGHT_SPOT))
					{
						comp = new ComponentLightSpot(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }
				case ComponentType::LIGHT_POINT: {

					if (!selected->HasComponent(ComponentType::LIGHT_POINT))
					{
						comp = new ComponentLightPoint(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }

				case ComponentType::CAMERA: {
					if (!selected->HasComponent(ComponentType::CAMERA))
					{
						comp = new ComponentCamera(selected);
						selected->AddComponent(comp);
						App->renderer3D->selected_game_camera = (ComponentCamera*)comp;
					}
					else
						LOG_ENGINE("The selected object already has this component!");

					break; }

				case ComponentType::ANIMATOR: {
					if (!selected->HasComponent(ComponentType::ANIMATOR))
					{
						comp = new ComponentAnimator(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");
					break; }
				case ComponentType::PARTICLES: {
					if (!selected->HasComponent(ComponentType::PARTICLES))
					{
						comp = new ComponentParticleSystem(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");
					break; }
				case ComponentType::A_EMITTER: {
					if (!selected->HasComponent(ComponentType::A_EMITTER))
					{
						comp = new ComponentAudioEmitter(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");
					break; }

				case ComponentType::A_LISTENER: {
					if (!selected->HasComponent(ComponentType::A_LISTENER))
					{
						comp = new ComponentAudioListener(selected);
						selected->AddComponent(comp);
					}
					else
						LOG_ENGINE("The selected object already has this component!");
					break; }

				case ComponentType::CANVAS: {
					if (!selected->HasComponent(ComponentType::CANVAS))
					{
						comp = new ComponentCanvas(selected);
						selected->AddComponent(comp);
					}

					else
						LOG_ENGINE("The selected object already has this component!");

					break; }

				case ComponentType::UI_IMAGE: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						comp = new ComponentImage(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
					}

					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_BUTTON: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						Component* comp_emitter = nullptr;
						Component* comp_text = nullptr;

						GameObject* object_text = App->objects->CreateEmptyGameObject(nullptr);

						comp = new ComponentButton(selected);
						comp_emitter = new ComponentAudioEmitter(selected);
						comp_text = new ComponentText(object_text);

						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						dynamic_cast<ComponentUI*>(comp_text)->SetCanvas(canvas);

						selected->SetName("Button");
						selected->AddComponent(comp);
						selected->AddComponent(comp_emitter);

						object_text->SetName("Text");
						object_text->AddComponent(comp_text);

						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
						App->objects->ReparentGameObject(object_text, selected, false);
					}

					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_TEXT: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						comp = new ComponentText(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
					}
					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_CHECKBOX: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						Component* comp_emitter = nullptr;	
						comp = new ComponentCheckbox(selected);
						comp_emitter = new ComponentAudioEmitter(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						selected->AddComponent(comp_emitter);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
					}
					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_SLIDER: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						Component* comp_emitter = nullptr;
						comp = new ComponentSlider(selected);
						comp_emitter = new ComponentAudioEmitter(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						selected->AddComponent(comp_emitter);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);

					}
					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_BAR: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						comp = new ComponentBar(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
					}
					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::UI_ANIMATED_IMAGE: {
					if (!selected->HasComponent(ComponentType::UI))
					{
						ComponentCanvas* canvas = GetCanvas();
						comp = new ComponentAnimatedImage(selected);
						dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
						selected->AddComponent(comp);
						App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
					}
					else
						LOG_ENGINE("The selected object already has Component UI!");
					break; }
				case ComponentType::BOX_COLLIDER: {
						comp = new ComponentBoxCollider(selected);
						selected->AddComponent(comp);
					break; }
				case ComponentType::SPHERE_COLLIDER: {
						comp = new ComponentSphereCollider(selected);
						selected->AddComponent(comp);
					break; }
				case ComponentType::CAPSULE_COLLIDER: {
						comp = new ComponentCapsuleCollider(selected);
						selected->AddComponent(comp);
					break; }
				case ComponentType::CONVEX_HULL_COLLIDER: {
						comp = new ComponentConvexHullCollider(selected);
						selected->AddComponent(comp);
					break; }
				case ComponentType::RIGID_BODY: {
					if (!selected->HasComponent(ComponentType::RIGID_BODY))
					{
						comp = new ComponentRigidBody(selected);
						selected->AddComponent(comp);
					}
					break; }
				case ComponentType::CHARACTER_CONTROLLER: {
					if (!selected->HasComponent(ComponentType::CHARACTER_CONTROLLER))
					{
						comp = new ComponentCharacterController(selected);
						selected->AddComponent(comp);
					}
					break; }
				}

				if (comp != nullptr) {
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, comp);
					added = true;
				}
			}
			if (added) {
				component = 0;
			}
		}
	}
}

void PanelInspector::ShowModelImportSettings(ResourceModel* model)
{
	if (model->animations_attached.size() > 0)
	{
		static char anim_name[MAX_PATH] = "Name";

		for each (ResourceAnimation * anim in model->animations_attached)
		{
			ImGui::PushID(anim);
			strcpy_s(anim_name, 100, anim->name.data());
			if (ImGui::InputText("Clip Name", anim_name, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				anim->name = anim_name;
			}

			//bool is_dae = (anim->ticks_per_second == 1);

			uint max_tick = /*is_dae ? (uint)(anim->max_tick * DAE_FPS) : */(int)anim->max_tick;
			int start_tick = /*is_dae ? (int)(anim->start_tick * DAE_FPS) : */(int)anim->start_tick;
			int end_tick = /*is_dae ? (int)(anim->end_tick * DAE_FPS) : */(int)anim->end_tick;

			if (ImGui::DragInt("Start", &start_tick, 1.0F, 0, end_tick - 1))
			{
				if (start_tick >= 0 && start_tick < end_tick)
				{
					anim->start_tick = /*is_dae ? (float)start_tick / DAE_FPS :*/ (uint)start_tick;
				}
			}
			if (ImGui::DragInt("End", &end_tick, 1.0F, start_tick + 1, max_tick))
			{
				if (end_tick > start_tick && end_tick <= max_tick)
				{
					anim->end_tick = /*is_dae ? (float)end_tick / DAE_FPS :*/ (uint)end_tick;
				}
			}
				
			ImGui::Checkbox("Loops", &anim->loops);
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::Button("+"))
		{
			ResourceAnimation* new_anim = new ResourceAnimation();
			new_anim->name = "New Clip";
			new_anim->max_tick = model->animations_attached[0]->max_tick;
			new_anim->end_tick = model->animations_attached[0]->max_tick;

			model->animations_attached.push_back(new_anim);
			App->resources->AddResource(new_anim);
		}
		ImGui::SameLine();
		if (ImGui::Button("-") && model->animations_attached.size() > 1)
		{
			App->resources->RemoveResource(model->animations_attached[model->animations_attached.size() - 1]);
			delete model->animations_attached[model->animations_attached.size() - 1];
			model->animations_attached.pop_back();
		}
		ImGui::Separator();
		if (ImGui::Button("Save")) {
			//TODO: FIX THIS MESS OF CODE :|	
			model->animations_attached[0]->LoadMemory();
			if (model->animations_attached.size() > 1)
			{
				for (int i = 1; i < model->animations_attached.size(); ++i)
				{
					model->animations_attached[i]->Copy(model->animations_attached[0]);
				}
			}
			model->UpdateAnimationInfo();
		}
	}
}

ComponentCanvas* PanelInspector::GetCanvas()
{
	ComponentCanvas* canvas = App->objects->GetRoot(true)->GetCanvas();
	if (canvas == nullptr) {
		GameObject* obj = new GameObject(App->objects->GetRoot(false));
		obj->SetName("Canvas");
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}

void PanelInspector::DropScript()
{
	// drop a node in the window, parent is base_game_object
	ImVec2 min_space = ImGui::GetWindowContentRegionMin();
	ImVec2 max_space = ImGui::GetWindowContentRegionMax();

	min_space.x += ImGui::GetWindowPos().x;
	min_space.y += ImGui::GetWindowPos().y;
	max_space.x += ImGui::GetWindowPos().x;
	max_space.y += ImGui::GetWindowPos().y;

	if (ImGui::BeginDragDropTargetCustom({ min_space.x,min_space.y, max_space.x,max_space.y }, ImGui::GetID(panel_name.data()))) {
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();
		if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
			FileNode* node = *(FileNode**)payload->Data;
			if (node->type == FileDropType::SCRIPT && ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover)) {
				if (App->objects->GetSelectedObjects().size() == 1) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					ResourceScript* script = (ResourceScript*)App->resources->GetResourceWithID(ID);
					path = node->path + node->name;
					JSONfilepack* scriptData = JSONfilepack::GetJSON(path.data());
					if (script != nullptr && scriptData != nullptr && scriptData->GetBoolean("HasData")) {
						JSONArraypack* structure = scriptData->GetArray("DataStructure");
						if (structure != nullptr) {
							structure->GetFirstNode();
							for (uint i = 0; i < structure->GetArraySize(); ++i) {
								if (strcmp(structure->GetString("DataName"), App->file_system->GetBaseFileName(node->name.data()).data()) == 0) {
									ComponentScript* comp_script = new ComponentScript(App->objects->GetSelectedObjects().back());
									comp_script->resourceID = script->GetID();
									comp_script->LoadData(structure->GetString("DataName"), structure->GetBoolean("UsesAlien"));
									ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, (void*)comp_script);
									if (Time::IsInGameState() && comp_script->need_alien && comp_script->data_ptr != nullptr) {
										Alien* alien = (Alien*)comp_script;
										if (alien != nullptr) {
											alien->Awake();
											alien->Start();
										}
									}
									break;
								}
								structure->GetAnotherNode();
							}
							delete scriptData;
						}
					}
				}

				ImGui::ClearDragDrop();
			}
		}
		ImGui::EndDragDropTarget();
	}
}

