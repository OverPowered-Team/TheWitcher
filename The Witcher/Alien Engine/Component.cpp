#include "Component.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleResources.h"
#include "SDL/include/SDL_assert.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentUI.h"
#include "ComponentImage.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ReturnZ.h"
#include "mmgr/mmgr.h"

Component::Component(GameObject* attach)
{
	ID = App->resources->GetRandomID();
	game_object_attached = attach;
}

Component::~Component()
{
}

bool Component::IsEnabled()
{
	return enabled;
}

void Component::SetEnable(bool enable)
{
	enabled = enable;

	(enabled) ? OnEnable() : OnDisable();
}

void Component::Destroy()
{
	not_destroy = false;
	App->objects->need_to_delete_objects = true;
}

void Component::ResetIDs()
{
	ID = App->resources->GetRandomID();
}

const ComponentType& Component::GetType() const
{
	return type;
}

std::string Component::EnumToString(ComponentType type)
{
	switch (type)
	{
	case ComponentType::NONE:
		return std::string("NONE");
		break;
	case ComponentType::TRANSFORM:
		return std::string("Transform");
		break;
	case ComponentType::MESH:
		return std::string("Mesh");
		break;
	case ComponentType::MATERIAL:
		return std::string("Material");
		break;
	case ComponentType::LIGHT_DIRECTIONAL:
		return std::string("Light directional");
		break;
	case ComponentType::LIGHT_SPOT:
		return std::string("Light spot");
		break;
	case ComponentType::LIGHT_POINT:
		return std::string("Light point");
		break;
	case ComponentType::CAMERA:
		return std::string("Camera");
		break;
	case ComponentType::BOX_COLLIDER:
		return std::string("Collider Box");
		break;
	case ComponentType::SPHERE_COLLIDER:
		return std::string("Colliders Sphere");
		break;
	case ComponentType::CAPSULE_COLLIDER:
		return std::string("Collider Capsule");
		break;
	case ComponentType::CONVEX_HULL_COLLIDER:
		return std::string("Collider Convex Hull");
		break;
	case ComponentType::RIGID_BODY:
		return std::string("Rigid Body");
		break;
	case ComponentType::POINT_CONSTRAINT:
		return std::string("Point Constraint");
		break;
	case ComponentType::CHARACTER_CONTROLLER:
		return std::string("Character Controller");
		break;
	case ComponentType::ANIMATOR:
		return std::string("Animator");
		break;
	case ComponentType::PARTICLES:
		return std::string("Particles");
		break;
	case ComponentType::A_EMITTER:
		return std::string("Audio Emitter");
		break;
	case ComponentType::A_LISTENER:
		return std::string("Audio Listener");
		break;
	case ComponentType::CANVAS:
		return std::string("Canvas");
		break;
	case ComponentType::UI_IMAGE:
		return std::string("UI Image");
		break;
	case ComponentType::UI_BUTTON:
		return std::string("UI Button");
		break;
	case ComponentType::UI_TEXT:
		return std::string("UI Text");
		break;
	case ComponentType::UI_CHECKBOX:
		return std::string("UI Checkbox");
		break;
	case ComponentType::UI_SLIDER:
		return std::string("UI Slider");
		break;
	case ComponentType::UI_BAR:
		return std::string("UI Bar");
		break;
	case ComponentType::UI_ANIMATED_IMAGE:
		return std::string("UI Animated Image");
		break;
	case ComponentType::DEFORMABLE_MESH:
		return std::string("Deformable Mesh");
		break;
	case ComponentType::BONE:
		return std::string("Bone");
		break;
	case ComponentType::SCRIPT:
		return std::string("Script");
		break;
	case ComponentType::UI:
		return std::string("UI");
		break;
	case ComponentType::CURVE:
		return std::string("Curve");
		break;
	case ComponentType::MAX:
		return std::string("MAX");
		break;
	default:
		return std::string("Not valid");
		break;
	}
}

void Component::RightClickMenu(const char* collapsing_header_name)
{
	if (ImGui::BeginPopupContextItem(collapsing_header_name)) {

		if (ImGui::MenuItem("Reset")) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			Reset();
		}

		ImGui::Separator();
		
		if (ImGui::MenuItem("Copy Component")) {
			SDL_assert((uint)ComponentType::MAX == 4); // add new case here
			if (App->objects->component_in_copy != nullptr) {
				delete App->objects->component_in_copy;
				App->objects->component_in_copy = nullptr;
			}
			switch (type) { // add to switch new type
			case ComponentType::LIGHT_DIRECTIONAL:
				App->objects->component_in_copy = new ComponentLightDirectional(nullptr);
				break;
			case ComponentType::LIGHT_SPOT:
				App->objects->component_in_copy = new ComponentLightSpot(nullptr);
				break;
			case ComponentType::TRANSFORM:
				App->objects->component_in_copy = new ComponentTransform(nullptr);
				break;
			case ComponentType::MESH:
				App->objects->component_in_copy = new ComponentMesh(nullptr);
				break;
			case ComponentType::MATERIAL:
				App->objects->component_in_copy = new ComponentMaterial(nullptr);
				break;
			case ComponentType::CAMERA:
				App->objects->component_in_copy = new ComponentCamera(nullptr);
				break;
			case ComponentType::CANVAS:
				App->objects->component_in_copy = new ComponentCanvas(nullptr);
				break;
			case ComponentType::UI: {
				ComponentUI* ui = (ComponentUI*)game_object_attached->GetComponent(ComponentType::UI);
				switch (ui->ui_type) {
				case ComponentType::UI_IMAGE: {
					App->objects->component_in_copy = new ComponentImage(nullptr);
					break; }
				}
				break; }
			}
			App->objects->component_in_copy->SetComponent(this);
		}

		bool can_paste = false;

		if (App->objects->component_in_copy != nullptr && App->objects->component_in_copy->GetType() == type && App->objects->component_in_copy != this)
			can_paste = true;

		if (ImGui::MenuItem("Paste Component", nullptr, nullptr, can_paste)) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			SetComponent(App->objects->component_in_copy);
		}

		if (type != ComponentType::TRANSFORM) {
			ImGui::Separator();

			if (ImGui::MenuItem("Enable Component", nullptr, nullptr, !enabled)) {
				enabled = true;
				OnEnable();
			}

			if (ImGui::MenuItem("Disable Component", nullptr, nullptr, enabled)) {
				enabled = false;
				OnDisable();
			}

			if (ImGui::MenuItem("Delete Component", nullptr, nullptr)) {
				not_destroy = false;
			}

			ImGui::Separator();

			bool is_on_top = false;
			bool is_on_bottom = false;

			if (game_object_attached->components.at(1) == this)
				is_on_top = true;
			if (game_object_attached->components.back() == this)
				is_on_bottom = true;

			if (ImGui::MenuItem("Move One Up", nullptr, nullptr, !is_on_top)) {
				App->objects->MoveComponentUp(game_object_attached, this, false);
			}

			if (ImGui::MenuItem("Move On Top", nullptr, nullptr, !is_on_top)) {
				App->objects->MoveComponentUp(game_object_attached, this, true);
			}

			if (ImGui::MenuItem("Move One Down", nullptr, nullptr, !is_on_bottom)) {
				App->objects->MoveComponentDown(game_object_attached, this, false);
			}

			if (ImGui::MenuItem("Move On Bottom", nullptr, nullptr, !is_on_bottom)) {
				App->objects->MoveComponentDown(game_object_attached, this, true);
			}
		}
		
		ImGui::EndPopup();
	}
}
