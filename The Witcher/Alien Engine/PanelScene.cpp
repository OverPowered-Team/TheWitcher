#include "PanelScene.h"
#include "Color.h"
#include "imgui/imgui.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "imgui/imgui_internal.h"
#include "FileNode.h"
#include "PanelSceneSelector.h"
#include "ResourcePrefab.h"
#include "ComponentTransform.h"
#include "Prefab.h"
#include "ReturnZ.h"
#include "PanelGame.h"
#include "ModuleInput.h"
#include "ShortCutManager.h"
#include "ModuleCamera3D.h"
#include "ModuleResources.h"
#include "ModuleImporter.h"
#include "ComponentCurve.h"
#include "ModuleUI.h"
#include "ModuleRenderer3D.h"
#include "Time.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

PanelScene::PanelScene(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Scene", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	App->shortcut_manager->AddShortCut("Translate", SDL_SCANCODE_W, std::bind(&PanelScene::Translate, this));
	App->shortcut_manager->AddShortCut("Rotate", SDL_SCANCODE_E, std::bind(&PanelScene::Rotate, this));
	App->shortcut_manager->AddShortCut("Scale", SDL_SCANCODE_R, std::bind(&PanelScene::Scale, this));
	App->shortcut_manager->AddShortCut("World Mode", SDL_SCANCODE_W, std::bind(&PanelScene::ChangeWorldMode, this), SDL_SCANCODE_LSHIFT);
	App->shortcut_manager->AddShortCut("Local Mode", SDL_SCANCODE_L, std::bind(&PanelScene::ChangeLocalMode, this), SDL_SCANCODE_LSHIFT);
}

PanelScene::~PanelScene()
{
}

void PanelScene::PanelLogic()
{
	OPTICK_EVENT();
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || ImGuizmo::IsOver()) {
		ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);
	}
	else
		ImGui::Begin(panel_name.data(), &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	App->camera->is_scene_focused = ImGui::IsWindowFocused();

	viewport_min = ImGui::GetCursorScreenPos();
	viewport_max = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail();

	current_viewport_size = ImGui::GetContentRegionAvail();
	posX = ImGui::GetWindowPos().x + ImGui::GetCursorPosX();
	posY = ImGui::GetWindowPos().y + ImGui::GetCursorPosY();
	ImGui::Image((ImTextureID)App->camera->scene_viewport->GetTexture(), ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));
	width = current_viewport_size.x;
	height = current_viewport_size.y;
	App->camera->is_scene_hovered = ImGui::IsItemHovered();

	App->camera->scene_viewport->SetPos(float2(viewport_min.x, viewport_min.y));

	if (!(current_viewport_size == viewport_size))
	{
		viewport_size = current_viewport_size;
		App->camera->scene_viewport->SetSize(viewport_size.x, viewport_size.y);
	}


	App->camera->scene_viewport->active = enabled; 

	// drop a fbx/texture in the window
	ImVec2 min_space = ImGui::GetWindowContentRegionMin();
	ImVec2 max_space = ImGui::GetWindowContentRegionMax();

	min_space.x += ImGui::GetWindowPos().x;
	min_space.y += ImGui::GetWindowPos().y;
	max_space.x += ImGui::GetWindowPos().x;
	max_space.y += ImGui::GetWindowPos().y;

	// drop project files
	if (ImGui::BeginDragDropTargetCustom({ min_space.x,min_space.y, max_space.x,max_space.y }, ImGui::GetID(panel_name.data()))) {

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
		if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
			FileNode* node = *(FileNode**)payload->Data;

			// drop particle system
			if (node != nullptr && node->type == FileDropType::PARTICLES && !App->objects->GetSelectedObjects().empty()) {
				std::string file_path = node->path + node->name;
				std::string path = App->file_system->GetPathWithoutExtension(file_path);
				path += ".alienParticles";

				App->importer->ApplyParticleSystemToSelectedObject(path);
			}

			// drop texture
			if (node != nullptr && node->type == FileDropType::TEXTURE && !App->objects->GetSelectedObjects().empty()) {
				std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
				path += "_meta.alien";

				u64 ID = App->resources->GetIDFromAlienPath(path.data());

				ResourceTexture* texture_dropped = (ResourceTexture*)App->resources->GetResourceWithID(ID);

				if (texture_dropped != nullptr) {
					App->importer->ApplyTextureToSelectedObject(texture_dropped);
				}
			}

			// drop shader
			if (node != nullptr && node->type == FileDropType::SHADER && !App->objects->GetSelectedObjects().empty()) {
				std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
				path += "_meta.alien";

				u64 ID = App->resources->GetIDFromAlienPath(path.data());

				ResourceShader* shader_dropped = (ResourceShader*)App->resources->GetResourceWithID(ID);

				if (shader_dropped != nullptr) {
					//App->importer->ApplyShaderToSelectedObject(shader_dropped); TODO
				}
			}

			// drop material
			if (node != nullptr && node->type == FileDropType::MATERIAL && !App->objects->GetSelectedObjects().empty()) {
				std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
				path += "_meta.alien";

				u64 ID = App->resources->GetIDFromAlienPath(path.data());

				ResourceMaterial* material_dropped = (ResourceMaterial*)App->resources->GetResourceWithID(ID);

				if (material_dropped != nullptr) {
					App->importer->ApplyMaterialToSelectedObject(material_dropped); 
				}
			}

			// drop prefab
			if (node != nullptr && node->type == FileDropType::PREFAB) {
				std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name) + "_meta.alien";
				u64 ID = App->resources->GetIDFromAlienPath(path.data());
				if (ID != 0) {
					ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(ID);
					GameObject* prefab_parent = nullptr;
					if (App->objects->current_scenes.empty() || App->objects->GetSelectedObjects().empty()) {
						if (App->objects->prefab_scene) {
							prefab_parent = App->objects->GetRoot(false);
						}
						else {
							prefab_parent = App->objects->GetRoot(true)->children.front();
						}
					}
					else {
						prefab_parent = App->objects->GetRoot(false);
					}
					prefab_parent = App->objects->GetRoot(false);
					prefab->ConvertToGameObjects(prefab_parent);
					if (Time::IsInGameState()) {
						Prefab::InitScripts(prefab_parent->children.back());
					}
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, prefab_parent->children.back());
				}
			}

			// drop model
			if (node != nullptr && node->type == FileDropType::MODEL3D) {
				std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
				path += "_meta.alien";

				u64 ID = App->resources->GetIDFromAlienPath(path.data());

				std::string meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";

				if (!App->resources->CreateNewModelInstanceOf(meta_path.data())) { // if it goes here it is because this file wasn't imported yet, so import it now

					App->importer->LoadModelFile(std::string(node->path + node->name).data(), nullptr);
					ID = App->resources->GetIDFromAlienPath(path.data());
					meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";
					App->resources->CreateNewModelInstanceOf(meta_path.data());
				}
			}

			// drop scene
			if (node != nullptr && node->type == FileDropType::SCENE && !App->objects->prefab_scene) {
				static char curr_dir[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, curr_dir);
				std::string full_scene_path = curr_dir + std::string("/") + node->path + node->name;
				App->ui->panel_scene_selector->LoadSceneWithPath(full_scene_path.data());
			}
		}

		ImGui::EndDragDropTarget();
	}

	GuizmosControls();
	GuizmosLogic();

	ImDrawList* list = ImGui::GetWindowDrawList();
	if (list != nullptr && list->CmdBuffer.size() > 1) {
		is_window_being_rendered = true;
		App->camera->scene_viewport->active = true;
	}
	else {
		App->camera->scene_viewport->active = false;
		is_window_being_rendered = false;
	}

	if (App->renderer3D->selected_game_camera != nullptr && is_window_being_rendered && App->objects->GetSelectedObjects().size() == 1)
	{
		ImGui::SetNextWindowPos(ImVec2(max_space.x - 212, max_space.y - 154));
		ImGui::SetNextWindowSize(ImVec2(192, 134));
		ImGui::Begin("Camera Selected Preview", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing);
		Viewport* viewport = App->camera->selected_viewport;
		if (viewport->GetCamera() != App->renderer3D->selected_game_camera) {
			viewport->SetCamera(App->renderer3D->selected_game_camera);
		}
		if (viewport != nullptr) {
			ImGui::Image((ImTextureID)viewport->GetTexture(), { 176,99 }, { 0,1 }, { 1,0 });
		}
		ImGui::End();
		App->camera->selected_viewport->active = enabled;
	}
	else {
		App->camera->selected_viewport->active = false; // Active/disavtive with window active
	}

	ImGui::End();
}

void PanelScene::GuizmosLogic()
{
	if (!App->objects->GetSelectedObjects().empty()) 
	{
		if (gizmo_curve) {
			GizmoCurve();
		}
		else {
			std::list<GameObject*> selected = App->objects->GetSelectedObjects();

			float4x4 view_transposed = float4x4(App->camera->fake_camera->frustum.ViewMatrix()).Transposed();
			float4x4 projection_transposed = float4x4(App->camera->fake_camera->frustum.ProjectionMatrix()).Transposed();
			math::float4x4 object_transform_matrix;
			math::float3 globalPosition = float3::zero();
			math::float3 globalSize = float3::zero();


			if (selected.size() > 1)
			{
				for (auto item = selected.begin(); item != selected.end(); ++item)
				{
					if (*item != nullptr)
					{
						if ((*item)->is_static)
							return;

						globalPosition += (*item)->transform->GetGlobalMatrix().TranslatePart();
						globalSize += (*item)->transform->GetGlobalMatrix().GetScale();
					}
				}

				globalPosition /= selected.size();
				globalSize /= selected.size();
				object_transform_matrix = float4x4::FromTRS(globalPosition, math::Quat::identity(), globalSize).Transposed();
			}
			else
			{
				object_transform_matrix = (*selected.begin())->transform->GetGlobalMatrix().Transposed();
			}

			ImGuizmo::SetRect(viewport_min.x, viewport_min.y, current_viewport_size.x, current_viewport_size.y);
			ImGuizmo::SetDrawlist();

			ImGuizmo::Manipulate(view_transposed.ptr(), projection_transposed.ptr(), guizmo_operation, (guizmo_operation != ImGuizmo::OPERATION::SCALE) ? guizmo_mode : ImGuizmo::MODE::LOCAL, object_transform_matrix.ptr());

			static bool guizmo_return = true;
			static bool duplicate = false;

			if (!ImGui::IsAnyPopupActive() && ImGuizmo::IsUsing() && ImGui::IsWindowFocused())
			{
				if (!duplicate && (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)) {
					duplicate = true;
					App->objects->DuplicateObjects();
				}
				if (selected.size() > 1)
				{
					object_transform_matrix.Transpose();
					math::float3 transformPos = object_transform_matrix.TranslatePart();
					math::Quat resformQuat = object_transform_matrix.RotatePart().ToQuat();
					math::float3 transformScale = object_transform_matrix.GetScale();

					for (auto item = selected.begin(); item != selected.end(); ++item) {
						if (*item != nullptr)
						{
							if (guizmo_return)
								ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, (*item)->transform);

							math::float3 finalPos, finalScale;
							math::Quat finalRot;

							(*item)->transform->GetGlobalMatrix().Decompose(finalPos, finalRot, finalScale);

							finalPos += transformPos - globalPosition;
							finalRot = resformQuat * finalRot;
							finalScale += transformScale - globalSize;

							(*item)->transform->SetGlobalTransformation(math::float4x4::FromTRS(finalPos, finalRot, finalScale));

						}
						if (guizmo_return && (*item) == selected.back())
							guizmo_return = false;

					}
				}
				else
				{
					if (guizmo_return)
						ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, (*selected.begin())->transform);

					(*selected.begin())->transform->SetGlobalTransformation((*selected.begin())->parent->transform->global_transformation.Inverted() * object_transform_matrix.Transposed());

					if (guizmo_return) {
						guizmo_return = false;
					}
				}

			}
			else if (!guizmo_return) {
				guizmo_return = true;
				duplicate = false;
			}
		}
		
	}
}

void PanelScene::GizmoCurve()
{
	float4x4 view_transposed = float4x4(App->camera->fake_camera->frustum.ViewMatrix()).Transposed();
	float4x4 projection_transposed = float4x4(App->camera->fake_camera->frustum.ProjectionMatrix()).Transposed();
	math::float4x4 object_transform_matrix = float4x4::FromTRS(curve->curve.GetControlPoints()[curve_index],Quat::identity(), float3::one()).Transposed();

	ImGuizmo::SetRect(viewport_min.x, viewport_min.y, current_viewport_size.x, current_viewport_size.y);
	ImGuizmo::SetDrawlist();

	ImGuizmo::Manipulate(view_transposed.ptr(), projection_transposed.ptr(), guizmo_operation, (guizmo_operation != ImGuizmo::OPERATION::SCALE) ? guizmo_mode : ImGuizmo::MODE::LOCAL, object_transform_matrix.ptr());

	if (!ImGui::IsAnyPopupActive() && ImGuizmo::IsUsing() && ImGui::IsWindowFocused())
	{
		object_transform_matrix.Transpose();
		math::float3 transformPos = object_transform_matrix.TranslatePart();

		curve->curve.SetControlPointAt(curve_index, transformPos);
	}
}

void PanelScene::GuizmosControls()
{
	if (ImGuizmo::IsUsing()) {
		return;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) && (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) && (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT))
	{
		guizmo_mode = ImGuizmo::MODE::WORLD;
	}
	if ((App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) && (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN))
	{
		guizmo_mode = ImGuizmo::MODE::LOCAL;
	}
	

}
void PanelScene::ChangeWorldMode()
{
	if (ImGuizmo::IsUsing()) 
	{
		return;
	}

	if ((App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT))
	{
		guizmo_mode = ImGuizmo::MODE::WORLD;
	}
}
void PanelScene::ChangeLocalMode()
{
	if (ImGuizmo::IsUsing()) {
		return;
	}
		guizmo_mode = ImGuizmo::MODE::LOCAL;
}
void PanelScene::Translate()
{
	if (ImGuizmo::IsUsing()) {
		return;
	}
	if((App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT) && (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KEY_REPEAT))
		guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
}
void PanelScene::Rotate()
{
	if (ImGuizmo::IsUsing()) {
		return;
	}
		guizmo_operation = ImGuizmo::OPERATION::ROTATE;

}
void PanelScene::Scale()
{
	if (ImGuizmo::IsUsing()) {
		return;
	}
	guizmo_operation = ImGuizmo::OPERATION::SCALE;
}