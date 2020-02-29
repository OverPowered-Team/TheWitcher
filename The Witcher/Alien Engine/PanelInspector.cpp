#include "PanelInspector.h"
#include "ModuleObjects.h"
#include "ModuleRenderer3D.h"
#include "ModuleUI.h"
#include "PanelProject.h"
#include "ResourceScript.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentLight.h"
#include "ComponentAnimator.h"

#include "ResourceAnimation.h"
#include "ResourceModel.h"
#include "ResourceBone.h"
#include "ResourceMesh.h"

#include "ComponentAudioListener.h"
#include "ComponentAudioEmitter.h"
#include "ComponentReverbZone.h"
#include "ComponentParticleSystem.h"
#include "ComponentSlider.h"
#include "ComponentCanvas.h"
#include "ComponentUI.h"
#include "ComponentImage.h"
#include "ComponentBar.h"
#include "ComponentText.h"
#include "ComponentButton.h"
#include "ComponentCheckbox.h"

#include "ReturnZ.h"
#include "Alien.h"
#include "ComponentScript.h"

#include "mmgr/mmgr.h"

#include "ComponentBoxCollider.h"
#include "ComponentSphereCollider.h"
#include "ComponentCapsuleCollider.h"
#include "ComponentConvexHullCollider.h"
#include "ComponentRigidBody.h"

#include "Optick/include/optick.h"

PanelInspector::PanelInspector(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Inspector", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
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
					if (!(*item)->not_destroy) {
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
	}
	else if (App->ui->panel_project->GetSelectedFile())
	{
		FileNode* selected_file = App->ui->panel_project->GetSelectedFile();
		if (selected_file->type == FileDropType::MODEL3D)
		{
			ResourceModel* r_model = (ResourceModel*)App->resources->GetResourceWithID(App->resources->GetIDFromAlienPath(std::string(App->file_system->GetPathWithoutExtension(std::string(selected_file->path + selected_file->name)) + "_meta.alien").data()));
			ShowModelImportSettings(r_model);
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

	if (component == (uint)ComponentType::SCRIPT) {
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
				GameObject* obj = App->objects->GetSelectedObjects().back();
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
					std::get<0>(script_info) = "Return To Components";
					component = 0;
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, (void*)comp_script);
					if (Time::IsInGameState() && comp_script->need_alien && comp_script->data_ptr != nullptr) {
						Alien* alien = (Alien*)comp_script;
						if (alien != nullptr) {
							alien->Awake();
							alien->Start();
						}
					}
				}
				else {
					LOG_ENGINE("This script is already attached!");
				}
			}
			else {
				LOG_ENGINE("Select a script");
			}
		}
	}

	else {
		ImGui::Combo("##choose component", &component, 
			"Select Component\0Mesh\0Material\0Light\0Camera\0Box Collider\0Sphere Collider\0Capsule Collider\0ConvexHull Collider\0Rigid Body\0Animator\0Particle System\0Audio Emitter\0Audio Listener\0Audio Reverb\0Canvas\0Image\0Button\0Text\0Checkbox\0Slider\0Bar\0DeformableMesh\0Bone\0Script\0"); // SCRIPT MUST BE THE LAST ONE
		ImGui::SameLine();

		if (ImGui::Button("Add Component"))
		{
			Component* comp = nullptr;
			switch ((ComponentType)component)
			{

			case (ComponentType)0: {
				LOG_ENGINE("Select a Component!");
				break; }

			case ComponentType::MESH: {

				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::MESH))
				{
					comp = new ComponentMesh(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}

				else
					LOG_ENGINE("The selected object already has this component!");

				break; }

			case ComponentType::MATERIAL: {

				if ((!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::MATERIAL)) &&
					App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::MESH))
				{
					comp = new ComponentMaterial(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}

				else if (App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::MATERIAL))
				{
					LOG_ENGINE("The selected object already has this component!");
				}

				else
					LOG_ENGINE("The object needs a mesh to have a material!");

				break; }

			case ComponentType::LIGHT: {

				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::LIGHT))
				{
					comp = new ComponentLight(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				else
					LOG_ENGINE("The selected object already has this component!");

				break; }

			case ComponentType::CAMERA: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::CAMERA))
				{
					comp = new ComponentCamera(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
					App->renderer3D->selected_game_camera = (ComponentCamera*)comp;
				}
				else
					LOG_ENGINE("The selected object already has this component!");

				break; }

			case ComponentType::ANIMATOR: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::ANIMATOR))
				{
					comp = new ComponentAnimator(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				else
					LOG_ENGINE("The selected object already has this component!");
				break; }
			case ComponentType::PARTICLES: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::PARTICLES))
				{
					comp = new ComponentParticleSystem(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				else
					LOG_ENGINE("The selected object already has this component!");
				break; }
			case ComponentType::A_EMITTER: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::A_EMITTER))
				{
					comp = new ComponentAudioEmitter(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				else
					LOG_ENGINE("The selected object already has this component!");
				break; }

				case ComponentType::A_LISTENER: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::A_LISTENER))
				{
					comp = new ComponentAudioListener(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				else
					LOG_ENGINE("The selected object already has this component!");
				break; }

			case ComponentType::A_REVERB: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::A_REVERB))
				{
					comp = new ComponentReverbZone(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}

				else
					LOG_ENGINE("The selected object already has this component!");
				break; }

				case ComponentType::CANVAS: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::CANVAS))
				{
					comp = new ComponentCanvas(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}

				else
					LOG_ENGINE("The selected object already has this component!");

				break; }

			case ComponentType::UI_IMAGE: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();
					comp = new ComponentImage(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
				}

				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::UI_BUTTON: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();
					comp = new ComponentButton(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
				}

				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::UI_TEXT: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();
					comp = new ComponentText(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
				}
				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::UI_CHECKBOX: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();

					comp = new ComponentCheckbox(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
				}
				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::UI_SLIDER: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();
					comp = new ComponentSlider(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);

				}
				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::UI_BAR: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::UI))
				{
					ComponentCanvas* canvas = GetCanvas();
					GameObject* selected = App->objects->GetSelectedObjects().back();
					comp = new ComponentBar(selected);
					dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
					selected->AddComponent(comp);
					App->objects->ReparentGameObject(selected, canvas->game_object_attached, false);
				}
				else
					LOG_ENGINE("The selected object already has Component UI!");
				break; }
			case ComponentType::BOX_COLLIDER: {
				if (App->objects->GetSelectedObjects().back()->GetComponent<ComponentCollider>() == nullptr)
				{
					comp = new ComponentBoxCollider(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				break; }
			case ComponentType::SPHERE_COLLIDER: {
				if (App->objects->GetSelectedObjects().back()->GetComponent<ComponentCollider>() == nullptr)
				{
					comp = new ComponentSphereCollider(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				break; }
			case ComponentType::CAPSULE_COLLIDER: {
				if (App->objects->GetSelectedObjects().back()->GetComponent<ComponentCollider>() == nullptr)
				{
					comp = new ComponentCapsuleCollider(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				break; }
			case ComponentType::CONVEX_HULL_COLLIDER: {
				if (App->objects->GetSelectedObjects().back()->GetComponent<ComponentCollider>() == nullptr)
				{
					comp = new ComponentSphereCollider(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				break; }
			case ComponentType::RIGID_BODY: {
				if (!App->objects->GetSelectedObjects().back()->HasComponent(ComponentType::RIGID_BODY))
				{
					comp = new ComponentRigidBody(App->objects->GetSelectedObjects().back());
					App->objects->GetSelectedObjects().back()->AddComponent(comp);
				}
				break; }
			}


			if (comp != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, comp);
			}
			component=0;
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
			int start_tick = (int)anim->start_tick;
			int end_tick = (int)anim->end_tick;
			if (ImGui::DragInt("Start", &start_tick, 1.0F, 0, anim->end_tick - 1))
				if (start_tick >= 0 && start_tick < anim->end_tick) anim->start_tick = (uint)start_tick;
			if (ImGui::DragInt("End", &end_tick, 1.0F, anim->start_tick + 1, anim->max_tick))
				if (end_tick > anim->start_tick&& end_tick <= anim->max_tick) anim->end_tick = (uint)end_tick;
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
		obj->AddComponent(new ComponentTransform(obj, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 }));
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}

