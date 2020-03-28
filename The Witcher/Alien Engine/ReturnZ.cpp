#include "ReturnZ.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentTransform.h"
#include "ResourceMesh.h"
#include "ComponentMaterial.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentLightPoint.h"
#include "ResourceScript.h"
#include "ResourceTexture.h"
#include "Maths.h"
#include "Octree.h"
#include "PanelTextEditor.h"
#include "ModuleUI.h"
#include "ModuleResources.h"
#include "ComponentImage.h"
#include "ComponentUI.h"
#include "ComponentBar.h"
#include "ComponentButton.h"
#include "ComponentCheckbox.h"
#include "ComponentSlider.h"
#include "ComponentScript.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "mmgr/mmgr.h"

bool ReturnZ::eraseY = false;

void ReturnZ::SetAction(const ReturnActions& type, void* data)
{
	switch (type)
	{
	case ReturnActions::DELETE_OBJECT: {
		ActionDeleteObject* object = new ActionDeleteObject();
		object->type = ReturnActions::DELETE_OBJECT;
		ReturnZ::SetDeleteObject((GameObject*)data, object);
		action = object;
		break; }
	case ReturnActions::ADD_OBJECT: {
		ActionAddObject* object = new ActionAddObject();
		object->type = ReturnActions::ADD_OBJECT;
		object->objectID = static_cast<GameObject*>(data)->ID;
		action = object;
		break; }
	case ReturnActions::CHANGE_COMPONENT: {
		ActionComponent* comp = new ActionComponent();
		comp->type = ReturnActions::CHANGE_COMPONENT;
		Component* component = (Component*)data;
		CompZ::SetCompZ(component, &comp->comp);
		if (comp->comp != nullptr) {
			action = comp;
		}
		break; }
	case ReturnActions::DELETE_COMPONENT: {
		ActionComponent* comp = new ActionComponent();
		comp->type = ReturnActions::DELETE_COMPONENT;
		Component* component = (Component*)data;
		CompZ::SetCompZ(component, &comp->comp);
		if (comp->comp != nullptr) {
			action = comp;
		}
		break; }
	case ReturnActions::ADD_COMPONENT: {
		ActionAddComponent* comp = new ActionAddComponent();
		comp->type = ReturnActions::ADD_COMPONENT;
		Component* component = (Component*)data;
		comp->compID = component->ID;
		comp->objectID = component->game_object_attached->ID;
		action = comp;
		break; }
	case ReturnActions::REPARENT_HIERARCHY: {
		ActionReparent* reparent = new ActionReparent();
		reparent->type = ReturnActions::REPARENT_HIERARCHY;
		GameObject* object = (GameObject*)data;
		reparent->objectID = object->ID;
		reparent->parentID = object->parent->ID;
		action = reparent;
		break; }
	default:
		break;
	}
}

ReturnZ::~ReturnZ()
{
	if (action != nullptr)
		delete action;
}

void ReturnZ::AddNewAction(const ReturnActions& type, void* data, bool user)
{
#ifndef GAME_VERSION
	if (App->objects->ignore_cntrlZ || App->ui->panel_text_editor->IsFocused())
		return;
	if (ReturnZ::eraseY && user) {
		/*for (uint i = 0; i < App->objects->fordward_actions.size(); ++i) {
			ReturnZ* act = App->objects->fordward_actions.top();
			delete act;
			act = nullptr;
			App->objects->fordward_actions.pop();
		}*/
		while (!App->objects->fordward_actions.empty()) {
			ReturnZ* act = App->objects->fordward_actions.top();
			delete act;
			act = nullptr;
			App->objects->fordward_actions.pop();
		}
	}
	ReturnZ* ret = new ReturnZ();
	ret->SetAction(type, data);
	if (ret->action != nullptr) {
		App->objects->return_actions.push(ret);
	}
	else {
		delete ret;
	}
#endif
}

void ReturnZ::AddNewFordwarAction(const ReturnActions& type, void* data)
{
#ifndef GAME_VERSION
	if (App->objects->ignore_cntrlZ || App->ui->panel_text_editor->IsFocused())
		return;
	ReturnZ* ret = new ReturnZ();
	ret->SetAction(type, data);
	if (ret->action != nullptr) {
		App->objects->fordward_actions.push(ret);
	}
	else {
		delete ret;
	}
#endif
}

void ReturnZ::GoBackOneAction()
{
#ifndef GAME_VERSION
	if (App->objects->return_actions.empty() || Time::IsPlaying() || App->ui->panel_text_editor->IsFocused())
		return;
	ReturnZ::eraseY = true;
	ReturnZ* to_return = App->objects->return_actions.top();
	App->objects->return_actions.pop();

	while (!ReturnZ::DoAction(to_return, false)) {
		if (App->objects->return_actions.empty()) {
			break;
		}
		to_return = App->objects->return_actions.top();
		App->objects->return_actions.pop();
	}
#endif
}

void ReturnZ::GoFordwardOneAction()
{
#ifndef GAME_VERSION
	if (App->objects->fordward_actions.empty() || Time::IsPlaying() || App->ui->panel_text_editor->IsFocused())
		return;

	ReturnZ* to_return = App->objects->fordward_actions.top();
	App->objects->fordward_actions.pop();

	while (!ReturnZ::DoAction(to_return, true)) {
		if (App->objects->fordward_actions.empty()) {
			break;
		}
		to_return = App->objects->fordward_actions.top();
		App->objects->fordward_actions.pop();
	}
#endif
}

bool ReturnZ::DoAction(ReturnZ* action, bool is_fordward)
{
	bool ret = false;
	App->objects->in_cntrl_Z = true;

	// if new case is added, remember to ret = true if succed
	switch (action->action->type) {
	case ReturnActions::DELETE_OBJECT: {
		ActionDeleteObject* object = (ActionDeleteObject*)action->action;
		ReturnZ::CreateObject(object);

		GameObject* obj = App->objects->GetGameObjectByID(object->object->ID);

		if (obj != nullptr) {
			if (is_fordward) {
				ReturnZ::AddNewAction(ReturnActions::ADD_OBJECT, obj, false);
			}
			else {
				ReturnZ::AddNewFordwarAction(ReturnActions::ADD_OBJECT, obj);
			}
			ret = true;
		}

		break; }
	case ReturnActions::ADD_OBJECT: {
		ActionAddObject* object = (ActionAddObject*)action->action;
		GameObject* to_delete = App->objects->GetGameObjectByID(object->objectID);
		if (to_delete != nullptr) {
			to_delete->ToDelete();
			ret = true;
			if (is_fordward) {
				ReturnZ::AddNewAction(ReturnActions::DELETE_OBJECT, App->objects->GetGameObjectByID(object->objectID), false);
			}
			else {
				ReturnZ::AddNewFordwarAction(ReturnActions::DELETE_OBJECT, App->objects->GetGameObjectByID(object->objectID));
			}
		}
		break; }
	case ReturnActions::CHANGE_COMPONENT: {
		ActionComponent* comp = (ActionComponent*)action->action;

		GameObject* obj = App->objects->GetGameObjectByID(comp->comp->objectID);

		if (obj != nullptr) {
			Component* component = obj->GetComponentWithID(comp->comp->compID);
			if (component != nullptr) {
				ret = true;
				if (is_fordward) {
					ReturnZ::AddNewAction(ReturnActions::CHANGE_COMPONENT, component, false);
				}
				else {
					ReturnZ::AddNewFordwarAction(ReturnActions::CHANGE_COMPONENT, component);
				}
				switch (comp->comp->type) {
				case ComponentType::TRANSFORM: {
					ComponentTransform* transform = (ComponentTransform*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(transform, comp->comp);
					if (App->objects->octree.Exists(transform->game_object_attached)) {
						App->objects->octree.Recalculate(nullptr);
					}
					break; }
				case ComponentType::MESH: {
					ComponentMesh* mesh = (ComponentMesh*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(mesh, comp->comp);
					break; }
				case ComponentType::CANVAS: {
					ComponentCanvas* canvas = (ComponentCanvas*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(canvas, comp->comp);
					break; }
				case ComponentType::MATERIAL: {
					ComponentMaterial* material = (ComponentMaterial*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(material, comp->comp);
					break; }
				case ComponentType::CAMERA: {
					ComponentCamera* camera = (ComponentCamera*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(camera, comp->comp);
					break; }
				case ComponentType::SCRIPT: {
					ComponentScript* script = (ComponentScript*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(script, comp->comp);
					break; }
				case ComponentType::LIGHT_DIRECTIONAL: {
					ComponentLightDirectional* light = (ComponentLightDirectional*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(light, comp->comp);
					break; }
				case ComponentType::LIGHT_SPOT: {
					ComponentLightSpot* light = (ComponentLightSpot*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(light, comp->comp);
					break; }
				case ComponentType::LIGHT_POINT: {
					ComponentLightPoint* light = (ComponentLightPoint*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(light, comp->comp);
					break; }
				case ComponentType::PARTICLES: {
					ComponentParticleSystem* ps = (ComponentParticleSystem*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
					CompZ::SetComponent(ps, comp->comp);}
				case ComponentType::UI: {
					switch (comp->comp->ui_type) {
					case ComponentType::UI_IMAGE: {
						ComponentImage* image = (ComponentImage*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
						CompZ::SetComponent(image, comp->comp);
						break; }
					case ComponentType::UI_BAR: {
						ComponentBar* bar = (ComponentBar*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
						CompZ::SetComponent(bar, comp->comp);
						break; }
					case ComponentType::UI_CHECKBOX: {
						ComponentCheckbox * checkbox = (ComponentCheckbox*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
						CompZ::SetComponent(checkbox, comp->comp);
						break; }
					case ComponentType::UI_BUTTON: {
						ComponentButton* button = (ComponentButton*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
						CompZ::SetComponent(button, comp->comp);
						break; }
					case ComponentType::UI_SLIDER: {
						ComponentSlider* slider = (ComponentSlider*)App->objects->GetGameObjectByID(comp->comp->objectID)->GetComponentWithID(comp->comp->compID);
						CompZ::SetComponent(slider, comp->comp);
						break; }
					}
					break; }
				}
			}
		}
		break; }
	case ReturnActions::DELETE_COMPONENT: {
		ActionComponent* comp = (ActionComponent*)action->action;
		CompZ::AttachCompZToGameObject(comp->comp);

		GameObject* obj = App->objects->GetGameObjectByID(comp->comp->objectID);
		if (obj != nullptr) {
			Component* component = obj->GetComponentWithID(comp->comp->compID);
			if (component != nullptr) {
				if (is_fordward) {
					ReturnZ::AddNewAction(ReturnActions::ADD_COMPONENT, component, false);
				}
				else {
					ReturnZ::AddNewFordwarAction(ReturnActions::ADD_COMPONENT, component);
				}
				ret = true;
			}
		}
		break; }
	case ReturnActions::ADD_COMPONENT: {
		ActionAddComponent* comp = (ActionAddComponent*)action->action;
		GameObject* obj = App->objects->GetGameObjectByID(comp->objectID);
		if (obj != nullptr) {
			Component* component = obj->GetComponentWithID(comp->compID);
			if (component != nullptr) {
				std::vector<Component*>::iterator item = obj->components.begin();
				for (; item != obj->components.end(); ++item) {
					if (*item != nullptr && *item == component) {

						if (is_fordward) {
							ReturnZ::AddNewAction(ReturnActions::DELETE_COMPONENT, component, false);
						}
						else {
							ReturnZ::AddNewFordwarAction(ReturnActions::DELETE_COMPONENT, component);
						}

						delete component;
						component = nullptr;
						obj->components.erase(item);
						ret = true;
						break;
					}
				}
			}
		}
		break; }
	case ReturnActions::REPARENT_HIERARCHY: {
		ActionReparent* reparent = (ActionReparent*)action->action;
		GameObject* obj = App->objects->GetGameObjectByID(reparent->objectID);
		GameObject* parent = App->objects->GetGameObjectByID(reparent->parentID);

		if (obj != nullptr && parent != nullptr) {
			if (is_fordward) {
				ReturnZ::AddNewAction(ReturnActions::REPARENT_HIERARCHY, obj, false);
			}
			else {
				ReturnZ::AddNewFordwarAction(ReturnActions::REPARENT_HIERARCHY, obj);
			}
			App->objects->ReparentGameObject(obj, parent, false);
			ret = true;
		}
		break; }
	}
	App->objects->in_cntrl_Z = false;
	delete action;

	return ret;
}

void ReturnZ::SetDeleteObject(GameObject* obj, ActionDeleteObject* to_fill)
{
	to_fill->object = new ObjZ();
	to_fill->object->prefabID = obj->GetPrefabID();
	to_fill->object->prefab_locked = obj->prefab_locked;
	to_fill->object->enabled = obj->enabled;
	to_fill->object->is_static = obj->is_static;
	to_fill->object->ID = obj->ID;
	to_fill->object->parentID = obj->parent->ID;
	to_fill->object->parent_selected = obj->IsParentSelected();
	to_fill->object->selected = obj->IsSelected();
	to_fill->object->parent_enabled = obj->IsParentEnabled();
	to_fill->object->name = std::string(obj->GetName()).data();
	
	if (!obj->components.empty()) {
		std::vector<Component*>::iterator item = obj->components.begin();
		for (; item != obj->components.end(); ++item) {
			if (*item != nullptr) {
				CompZ* comp = nullptr;
				switch ((*item)->GetType()) {
				case ComponentType::TRANSFORM: {
					CompTransformZ* transZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&transZ);
					comp = transZ;
					break; }
				case ComponentType::MESH: {
					CompMeshZ* meshZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&meshZ);
					comp = meshZ;
					break; }
				case ComponentType::CANVAS: {
					CompZ* canvasZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&canvasZ);
					comp = canvasZ;
					break; }
				case ComponentType::MATERIAL: {
					CompMaterialZ* materialZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&materialZ);
					comp = materialZ;
					break; }
				case ComponentType::LIGHT_DIRECTIONAL: {
					CompLightZ* lightZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&lightZ);
					comp = lightZ;
					break; }
				case ComponentType::LIGHT_SPOT: {
					CompLightZ* lightZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ * *)& lightZ);
					comp = lightZ;
					break; }
				case ComponentType::CAMERA: {
					CompCameraZ* cameraZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&cameraZ);
					comp = cameraZ;
					break; }
				case ComponentType::SCRIPT: {
					CompScriptZ* scriptZ = nullptr;
					CompZ::SetCompZ((*item), (CompZ**)&scriptZ);
					comp = scriptZ;
					break; }
				case ComponentType::UI: {
					ComponentUI* ui = (ComponentUI*)*item;
					switch (ui->ui_type) {
					case ComponentType::UI_IMAGE: {
						CompImageZ* imageZ = nullptr;
						CompZ::SetCompZ((*item), (CompZ**)&imageZ);
						comp = imageZ;
						break; }
					case ComponentType::UI_BAR: {
						CompBarZ* barZ = nullptr;
						CompZ::SetCompZ((*item), (CompZ**)&barZ);
						comp = barZ;
						break; }
					case ComponentType::UI_CHECKBOX: {
						CompCheckboxZ* checkZ = nullptr;
						CompZ::SetCompZ((*item), (CompZ**)&checkZ);
						comp = checkZ;
						break; }
					case ComponentType::UI_BUTTON: {
						CompButtonZ* buttonZ = nullptr;
						CompZ::SetCompZ((*item), (CompZ**)&buttonZ);
						comp = buttonZ;
						break; }
					case ComponentType::UI_SLIDER: {
						CompSliderZ* sliderZ = nullptr;
						CompZ::SetCompZ((*item), (CompZ**)&sliderZ);
						comp = sliderZ;
						break; }
					}
					break; }
				default:
					LOG_ENGINE("A component hasn't been saved");
					break;
				}
				if (comp != nullptr) {
					comp->type = (*item)->GetType();
					to_fill->object->comps.push_back(comp);
				}
			}
		}
	}

	if (!obj->children.empty()) {
		std::vector<GameObject*>::iterator item = obj->children.begin();
		for (; item != obj->children.end(); ++item) {
			if (*item != nullptr) {
				ActionDeleteObject* obj = new ActionDeleteObject();
				ReturnZ::SetDeleteObject((*item), obj);
				to_fill->object->children.push_back(obj);
			}
		}
	}
}

void ReturnZ::CreateObject(ActionDeleteObject* obj)
{
	GameObject* parent = App->objects->GetGameObjectByID(obj->object->parentID);
	if (parent != nullptr) {
		GameObject* new_obj = new GameObject(true);
		new_obj->parent = parent;
		if (new_obj->parent != nullptr) {
			new_obj->parent->AddChild(new_obj);
		}
		new_obj->enabled = obj->object->enabled;
		new_obj->is_static = obj->object->is_static;
		if (new_obj->is_static) {
			App->objects->octree.Insert(new_obj, false);
		}
		new_obj->prefab_locked = obj->object->prefab_locked;
		new_obj->SetPrefab(obj->object->prefabID);
		new_obj->ID = obj->object->ID;
		new_obj->SetName(obj->object->name.data());
		if (obj->object->selected) {
			App->objects->SetNewSelectedObject(new_obj);
		}
		new_obj->parent_enabled = obj->object->parent_enabled;
		new_obj->parent_selected = obj->object->parent_selected;

		if (!obj->object->comps.empty()) {
			std::vector<CompZ*>::iterator item = obj->object->comps.begin();
			for (; item != obj->object->comps.end(); ++item) {
				if (*item != nullptr) {
					SDL_assert((uint)ComponentType::MAX == 26); // add new type to switch
					switch ((*item)->type)
					{
					case ComponentType::TRANSFORM: {
						CompTransformZ* transZ = (CompTransformZ*)(*item);
						ComponentTransform* transform = new ComponentTransform(new_obj);
						new_obj->transform = transform;
						CompZ::SetComponent(transform, transZ);
						new_obj->AddComponent(transform);
						break; }
					case ComponentType::MESH: {
						ComponentMesh* mesh = new ComponentMesh(new_obj);
						CompMeshZ* meshZ = (CompMeshZ*)(*item);
						CompZ::SetComponent(mesh, meshZ);
						new_obj->AddComponent(mesh);
						break; }
					case ComponentType::CANVAS: {
						ComponentCanvas* canvas = new ComponentCanvas(new_obj);
						CompZ::SetComponent(canvas, (*item));
						new_obj->AddComponent(canvas);
						break; }
					case ComponentType::MATERIAL: {
						ComponentMaterial* material = new ComponentMaterial(new_obj);
						CompMaterialZ* materialZ = (CompMaterialZ*)(*item);
						CompZ::SetComponent(material, materialZ);
						new_obj->AddComponent(material);
						break; }
					case ComponentType::LIGHT_DIRECTIONAL: {
						ComponentLightDirectional* light = new ComponentLightDirectional(new_obj);
						CompLightZ* lightZ = (CompLightZ*)(*item);
						CompZ::SetComponent(light, lightZ);
						new_obj->AddComponent(light);
						break; }
					case ComponentType::LIGHT_SPOT: {
						ComponentLightSpot* light = new ComponentLightSpot(new_obj);
						CompLightZ* lightZ = (CompLightZ*)(*item);
						CompZ::SetComponent(light, lightZ);
						new_obj->AddComponent(light);
						break; }
					case ComponentType::LIGHT_POINT: {
						ComponentLightPoint* light = new ComponentLightPoint(new_obj);
						CompLightZ* lightZ = (CompLightZ*)(*item);
						CompZ::SetComponent(light, lightZ);
						new_obj->AddComponent(light);
						break; }
					case ComponentType::CAMERA: {
						ComponentCamera* camera = new ComponentCamera(new_obj);
						CompCameraZ* cameraZ = (CompCameraZ*)(*item);
						CompZ::SetComponent(camera, cameraZ);
						new_obj->AddComponent(camera);
						break; }
					case ComponentType::SCRIPT: {
						ComponentScript* script = new ComponentScript(new_obj);
						CompScriptZ* scriptZ = (CompScriptZ*)(*item);
						CompZ::SetComponent(script, scriptZ);
						break; }
					case ComponentType::UI: {
						switch ((*item)->ui_type) {
						case ComponentType::UI_IMAGE: {
							ComponentImage* image = new ComponentImage(new_obj);
							CompImageZ* imageZ = (CompImageZ*)(*item);
							CompZ::SetComponent(image, imageZ);
							new_obj->AddComponent(image);
							break; }
						case ComponentType::UI_BAR: {
							ComponentBar* bar = new ComponentBar(new_obj);
							CompBarZ* barZ = (CompBarZ*)(*item);
							CompZ::SetComponent(bar, barZ);
							new_obj->AddComponent(bar);
							break; }
						case ComponentType::UI_CHECKBOX: {
							ComponentCheckbox* check = new ComponentCheckbox(new_obj);
							CompCheckboxZ* checkZ = (CompCheckboxZ*)(*item);
							CompZ::SetComponent(check, checkZ);
							new_obj->AddComponent(check);
							break; }
						case ComponentType::UI_BUTTON: {
							ComponentButton* button = new ComponentButton(new_obj);
							CompButtonZ* buttonZ = (CompButtonZ*)(*item);
							CompZ::SetComponent(button, buttonZ);
							new_obj->AddComponent(button);
							break; }
						case ComponentType::UI_SLIDER: {
							ComponentSlider* slider = new ComponentSlider(new_obj);
							CompSliderZ* sliderZ = (CompSliderZ*)(*item);
							CompZ::SetComponent(slider, sliderZ);
							new_obj->AddComponent(slider);
							break; }
						}
						break; }
					default:
						break;
					}
				}
			}
		}

		if (!obj->object->children.empty()) {
			std::vector<ActionDeleteObject*>::iterator item = obj->object->children.begin();
			for (; item != obj->object->children.end(); ++item) {
				if (*item != nullptr) {
					ReturnZ::CreateObject((*item));
				}
			}
		}
	}
}

void CompZ::SetCompZ(Component* component, CompZ** compZ)
{
	switch (component->GetType()) {
	case ComponentType::TRANSFORM: {
		CompTransformZ* transZ = new CompTransformZ();
		*compZ = transZ;
		ComponentTransform* transform = (ComponentTransform*)component;
		transZ->pos = transform->GetLocalPosition();
		transZ->scale = transform->GetLocalScale();
		transZ->rot = transform->GetLocalRotation();
		break; }
	case ComponentType::MESH: {
		ComponentMesh* mesh = (ComponentMesh*)component;
		CompMeshZ* meshZ = new CompMeshZ();
		*compZ = meshZ;
		if (mesh->mesh != nullptr) {
			if (mesh->mesh->GetID() != 0) {
				meshZ->resourceID = mesh->mesh->GetID();
			}
			else if (mesh->mesh->is_primitive) {
				meshZ->is_primitive = true;
				if (App->StringCmp("Cube", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::CUBE;
				}
				else if (App->StringCmp("Sphere", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::SPHERE_ALIEN;
				}
				else if (App->StringCmp("Dodecahedron", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::DODECAHEDRON;
				}
				else if (App->StringCmp("Icosahedron", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::ICOSAHEDRON;
				}
				else if (App->StringCmp("Octahedron", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::OCTAHEDRON;
				}
				else if (App->StringCmp("Rock", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::ROCK;
				}
				else if (App->StringCmp("Torus", mesh->mesh->GetName())) {
					meshZ->type = PrimitiveType::TORUS;
				}
			}
		}
			
		meshZ->objectID = mesh->game_object_attached->ID;
		meshZ->draw_AABB = mesh->draw_AABB;
		meshZ->draw_OBB = mesh->draw_OBB;
		meshZ->view_face_normals = mesh->view_face_normals;
		meshZ->view_vertex_normals = mesh->view_vertex_normals;
		meshZ->wireframe = mesh->wireframe;
		meshZ->view_mesh = mesh->view_mesh;
		break; }
	case ComponentType::MATERIAL: {
		ComponentMaterial* material = (ComponentMaterial*)component;
		CompMaterialZ* materialZ = new CompMaterialZ();
		*compZ = materialZ;
		/*if (material->GetTexture() != nullptr) {
			materialZ->resourceID = material->GetTexture()->GetID();
		}
		materialZ->objectID = material->game_object_attached->ID;
		materialZ->color = material->color;
		materialZ->texture_activated = material->texture_activated;*/
		break; }
	case ComponentType::LIGHT_DIRECTIONAL: {
		ComponentLightDirectional* light = (ComponentLightDirectional*)component;
		CompLightZ* lightZ = new CompLightZ();
		*compZ = lightZ;
		lightZ->objectID = light->game_object_attached->ID;
		break; }
	case ComponentType::LIGHT_SPOT: {
		ComponentLightSpot* light = (ComponentLightSpot*)component;
		CompLightZ* lightZ = new CompLightZ();
		*compZ = lightZ;
		lightZ->objectID = light->game_object_attached->ID;
		break; }
	case ComponentType::LIGHT_POINT: {
		ComponentLightPoint* light = (ComponentLightPoint*)component;
		CompLightZ* lightZ = new CompLightZ();
		*compZ = lightZ;
		lightZ->objectID = light->game_object_attached->ID;
		break; }
	case ComponentType::CANVAS: {
		ComponentCanvas* canvas = (ComponentCanvas*)component;
		CompZ* canvasZ = new CompZ();
		*compZ = canvasZ;
		break; }
	case ComponentType::SCRIPT: {
		ComponentScript* script = (ComponentScript*)component;
		CompScriptZ* scriptZ = new CompScriptZ();
		*compZ = scriptZ;
		scriptZ->resourceID = script->resourceID;
		scriptZ->data_name = script->data_name;
		scriptZ->need_alien = script->need_alien;
		break; }
	case ComponentType::CAMERA: {
		ComponentCamera* camera = (ComponentCamera*)component;
		CompCameraZ* cameraZ = new CompCameraZ();
		*compZ = cameraZ;
		cameraZ->camera_color_background = camera->camera_color_background;
		cameraZ->far_plane = camera->far_plane;
		cameraZ->horizontal_fov = camera->horizontal_fov;
		cameraZ->vertical_fov = camera->vertical_fov;
		cameraZ->is_fov_horizontal = camera->is_fov_horizontal;
		cameraZ->objectID = camera->game_object_attached->ID;
		cameraZ->near_plane = camera->near_plane;
		break; }
	case ComponentType::PARTICLES: {
		ComponentParticleSystem* ps = (ComponentParticleSystem*)component;
		CompParticleSystemZ* psZ = new CompParticleSystemZ();
		*compZ = psZ;
		psZ->duration = ps->particleSystem->emmitter.GetMaxLife();
		psZ->objectID = ps->game_object_attached->ID;
		//psZ->enabled = true;
		break;}
	case ComponentType::UI: {
		ComponentUI* ui = (ComponentUI*)component;
		switch (ui->ui_type) {
		case ComponentType::UI_IMAGE: {
			CompImageZ* imageZ = new CompImageZ();
			*compZ = imageZ;
			imageZ->color = ui->current_color;
			imageZ->resourceID = (ui->texture != nullptr) ? ui->texture->GetID() : 0;
			imageZ->ui_type = ComponentType::UI_IMAGE;
			break; }
		case ComponentType::UI_BAR: {
			ComponentBar* bar = (ComponentBar*)ui;
			CompBarZ* barZ = new CompBarZ();
			*compZ = barZ;
			barZ->color = ui->current_color;
			barZ->resourceID = (ui->texture != nullptr) ? ui->texture->GetID() : 0;
			barZ->resourceBarID = (bar->barTexture != nullptr) ? bar->barTexture->GetID() : 0;
			barZ->ui_type = ComponentType::UI_BAR;
			break; }
		case ComponentType::UI_CHECKBOX: {
			ComponentCheckbox* check = (ComponentCheckbox*)ui;
			CompCheckboxZ* checkZ = new CompCheckboxZ();
			*compZ = checkZ;
			checkZ->color = ui->current_color;
			checkZ->resourceID = (ui->texture != nullptr) ? ui->texture->GetID() : 0;
			checkZ->resourceCrossID = (check->crossTexture != nullptr) ? check->crossTexture->GetID() : 0;
			checkZ->resourceTickID = (check->tickTexture != nullptr) ? check->tickTexture->GetID() : 0;
			checkZ->ui_type = ComponentType::UI_CHECKBOX;
			break; }
		case ComponentType::UI_BUTTON: {
			CompButtonZ* buttonZ = new CompButtonZ();
			*compZ = buttonZ;
			buttonZ->color = ui->current_color;
			buttonZ->resourceID = (ui->texture != nullptr) ? ui->texture->GetID() : 0;
			buttonZ->ui_type = ComponentType::UI_BUTTON;
			break; }
		case ComponentType::UI_SLIDER: {
			ComponentSlider* slider = (ComponentSlider*)ui;
			CompSliderZ* sliderZ = new CompSliderZ();
			*compZ = sliderZ;
			sliderZ->color = ui->current_color;
			sliderZ->resourceID = (ui->texture != nullptr) ? ui->texture->GetID() : 0;
			sliderZ->resourceID = (slider->sliderTexture != nullptr) ? slider->sliderTexture->GetID() : 0;
			sliderZ->ui_type = ComponentType::UI_SLIDER;
			break; }
		}
		break; }
	default: {
		break;
	}
	}
	if (*compZ != nullptr) {
		(*compZ)->type = component->GetType();
		(*compZ)->objectID = component->game_object_attached->ID;
		(*compZ)->compID = component->ID;
		(*compZ)->enabled = component->IsEnabled();
	}
}

void CompZ::SetComponent(Component* component, CompZ* compZ)
{
	if (component == nullptr) {
		return;
	}

	switch (compZ->type) {
	case ComponentType::TRANSFORM: {
		ComponentTransform* transform = (ComponentTransform*)component;
		CompTransformZ* transZ = (CompTransformZ*)compZ;
		transform->local_position = transZ->pos;
		transform->local_rotation = transZ->rot;
		transform->local_scale = transZ->scale;
		transform->euler_rotation = transform->local_rotation.ToEulerXYZ();
		transform->euler_rotation.x = RadToDeg(transform->euler_rotation.x);
		transform->euler_rotation.y = RadToDeg(transform->euler_rotation.y);
		transform->euler_rotation.z = RadToDeg(transform->euler_rotation.z);
		transform->LookScale();
		transform->RecalculateTransform();
		transform->game_object_attached->transform = transform;
		break; }
	case ComponentType::MESH: {
		ComponentMesh* mesh = (ComponentMesh*)component;
		CompMeshZ* meshZ = (CompMeshZ*)compZ;
		if (meshZ->resourceID != 0) {
			mesh->mesh = (ResourceMesh*)App->resources->GetResourceWithID(meshZ->resourceID);
		}
		else if (meshZ->is_primitive) {
			switch (meshZ->type) {
			case PrimitiveType::CUBE: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::CUBE);
				break; }
			case PrimitiveType::SPHERE_ALIEN: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::SPHERE_ALIEN);
				break; }
			case PrimitiveType::DODECAHEDRON: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::DODECAHEDRON);
				break; }
			case PrimitiveType::ICOSAHEDRON: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::ICOSAHEDRON);
				break; }
			case PrimitiveType::OCTAHEDRON: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::OCTAHEDRON);
				break; }
			case PrimitiveType::ROCK: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::ROCK);
				break; }
			case PrimitiveType::TORUS: {
				mesh->mesh = App->resources->GetPrimitive(PrimitiveType::TORUS);
				break; }
			}
		}
		mesh->draw_AABB = meshZ->draw_AABB;
		mesh->draw_OBB = meshZ->draw_OBB;
		mesh->wireframe = meshZ->wireframe;
		mesh->view_mesh = meshZ->view_mesh;
		mesh->view_face_normals = meshZ->view_face_normals;
		mesh->view_vertex_normals = meshZ->view_vertex_normals;
		if (mesh->mesh != nullptr) {
			mesh->mesh->IncreaseReferences();
		}
		mesh->GenerateLocalAABB();
		mesh->RecalculateAABB_OBB();
		break; }
	case ComponentType::MATERIAL: {
		/*ComponentMaterial* material = (ComponentMaterial*)component;
		CompMaterialZ* materialZ = (CompMaterialZ*)compZ;
		if (materialZ->resourceID == 0) {
			material->SetTexture(nullptr);
		}
		else {
			material->SetTexture((ResourceTexture*)App->resources->GetResourceWithID(materialZ->resourceID));
		}*/
		/*material->texture_activated = materialZ->texture_activated;
		material->color = materialZ->color;*/
		break; }
	case ComponentType::LIGHT_DIRECTIONAL: {
		ComponentLightDirectional* light = (ComponentLightDirectional*)component;
		CompLightZ* lightZ = (CompLightZ*)compZ;
		break; }
	case ComponentType::LIGHT_SPOT: {
		ComponentLightSpot* light = (ComponentLightSpot*)component;
		CompLightZ* lightZ = (CompLightZ*)compZ;
		break; }
	case ComponentType::LIGHT_POINT: {
		ComponentLightPoint* light = (ComponentLightPoint*)component;
		CompLightZ* lightZ = (CompLightZ*)compZ;
		break; }
	case ComponentType::CANVAS: {
		ComponentCanvas* canvas = (ComponentCanvas*)component;
		CompZ* canvasZ = (CompZ*)compZ;
		break; }
	case ComponentType::UI: {
		switch (compZ->ui_type) {
		case ComponentType::UI_IMAGE: {
			ComponentImage* image = (ComponentImage*)component;
			CompImageZ* imageZ = (CompImageZ*)compZ;
			image->current_color = imageZ->color;
			if (imageZ->resourceID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(imageZ->resourceID);
				if (tex != nullptr) {
					image->SetTexture(tex);
				}
				else {
					image->ClearTexture();
				}
			}
			else {
				image->ClearTexture();
			}

			GameObject* p = image->game_object_attached->parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
					if (canvas != nullptr) {
						image->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					image->SetCanvas(nullptr);
				}
			}
			break; }
		case ComponentType::UI_BAR: {
			ComponentBar* bar = (ComponentBar*)component;
			CompBarZ* barZ = (CompBarZ*)compZ;
			bar->current_color = barZ->color;
			if (barZ->resourceID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(barZ->resourceID);
				if (tex != nullptr) {
					bar->SetTexture(tex);
				}
				else {
					bar->ClearTexture();
				}
			}
			else {
				bar->ClearTexture();
			}
			//----------------
			if (barZ->resourceBarID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(barZ->resourceBarID);
				if (tex != nullptr) {
					if (tex != nullptr && tex != bar->barTexture) {
						tex->IncreaseReferences();
						if (bar->barTexture != nullptr) {
							bar->barTexture->DecreaseReferences();
						}
						bar->barTexture = tex;
					}
				}
				else {
					if (bar->barTexture != nullptr) {
						bar->barTexture->DecreaseReferences();
						bar->barTexture = nullptr;
					}
				}
			}
			else {
				if (bar->barTexture != nullptr) {
					bar->barTexture->DecreaseReferences();
					bar->barTexture = nullptr;
				}
			}
			//---------------
			GameObject* p = bar->game_object_attached->parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
					if (canvas != nullptr) {
						bar->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					bar->SetCanvas(nullptr);
				}
			}
			break; }
		case ComponentType::UI_CHECKBOX: {
			ComponentCheckbox* checkbox = (ComponentCheckbox*)component;
			CompCheckboxZ* checkZ = (CompCheckboxZ*)compZ;
			checkbox->current_color = checkZ->color;
			if (checkZ->resourceID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(checkZ->resourceID);
				if (tex != nullptr) {
					checkbox->SetTexture(tex);
				}
				else {
					checkbox->ClearTexture();
				}
			}
			else {
				checkbox->ClearTexture();
			}
			//----------------
			if (checkZ->resourceCrossID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(checkZ->resourceCrossID);
				if (tex != nullptr) {
					if (tex != nullptr && tex != checkbox->crossTexture) {
						tex->IncreaseReferences();
						if (checkbox->crossTexture != nullptr) {
							checkbox->crossTexture->DecreaseReferences();
						}
						checkbox->crossTexture = tex;
					}
				}
				else {
					if (checkbox->crossTexture != nullptr) {
						checkbox->crossTexture->DecreaseReferences();
						checkbox->crossTexture = nullptr;
					}
				}
			}
			else {
				if (checkbox->crossTexture != nullptr) {
					checkbox->crossTexture->DecreaseReferences();
					checkbox->crossTexture = nullptr;
				}
			}
			//---------------
			if (checkZ->resourceTickID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(checkZ->resourceTickID);
				if (tex != nullptr) {
					if (tex != nullptr && tex != checkbox->tickTexture) {
						tex->IncreaseReferences();
						if (checkbox->tickTexture != nullptr) {
							checkbox->tickTexture->DecreaseReferences();
						}
						checkbox->tickTexture = tex;
					}
				}
				else {
					if (checkbox->tickTexture != nullptr) {
						checkbox->tickTexture->DecreaseReferences();
						checkbox->tickTexture = nullptr;
					}
				}
			}
			else {
				if (checkbox->tickTexture != nullptr) {
					checkbox->tickTexture->DecreaseReferences();
					checkbox->tickTexture = nullptr;
				}
			}
			//---------------
			GameObject* p = checkbox->game_object_attached->parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
					if (canvas != nullptr) {
						checkbox->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					checkbox->SetCanvas(nullptr);
				}
			}
			break; }

		case ComponentType::UI_BUTTON: {
			ComponentButton* button = (ComponentButton*)component;
			CompButtonZ* buttonZ = (CompButtonZ*)compZ;
			button->current_color = buttonZ->color;
			if (buttonZ->resourceID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(buttonZ->resourceID);
				if (tex != nullptr) {
					button->SetTexture(tex);
				}
				else {
					button->ClearTexture();
				}
			}
			else {
				button->ClearTexture();
			}
			//----------------
			//---------------
			GameObject* p = button->game_object_attached->parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
					if (canvas != nullptr) {
						button->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					button->SetCanvas(nullptr);
				}
			}
			break; }
		case ComponentType::UI_SLIDER: {
			ComponentSlider* slider = (ComponentSlider*)component;
			CompSliderZ* sliderZ = (CompSliderZ*)compZ;
			slider->current_color = sliderZ->color;
			if (sliderZ->resourceID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(sliderZ->resourceID);
				if (tex != nullptr) {
					slider->SetTexture(tex);
				}
				else {
					slider->ClearTexture();
				}
			}
			else {
				slider->ClearTexture();
			}
			//----------------
			if (sliderZ->resourceSliderID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(sliderZ->resourceSliderID);
				if (tex != nullptr) {
					if (tex != nullptr && tex != slider->sliderTexture) {
						tex->IncreaseReferences();
						if (slider->sliderTexture != nullptr) {
							slider->sliderTexture->DecreaseReferences();
						}
						slider->sliderTexture = tex;
					}
				}
				else {
					if (slider->sliderTexture != nullptr) {
						slider->sliderTexture->DecreaseReferences();
						slider->sliderTexture = nullptr;
					}
				}
			}
			else {
				if (slider->sliderTexture != nullptr) {
					slider->sliderTexture->DecreaseReferences();
					slider->sliderTexture = nullptr;
				}
			}
			//---------------
			GameObject* p = slider->game_object_attached->parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
					if (canvas != nullptr) {
						slider->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					slider->SetCanvas(nullptr);
				}
			}
			break; }
		}
		break; }
	case ComponentType::SCRIPT: {
		ComponentScript* script = (ComponentScript*)component;
		CompScriptZ* scriptZ = (CompScriptZ*)compZ;
		ResourceScript* r_script = (ResourceScript*)App->resources->GetResourceWithID(scriptZ->resourceID);
		if (r_script != nullptr) {
			for (uint i = 0; i < r_script->data_structures.size(); ++i) {
				if (App->StringCmp(r_script->data_structures[i].first.data(), scriptZ->data_name.data())) {
					script->LoadData(scriptZ->data_name.data(), r_script->data_structures[i].second);
					script->resourceID = scriptZ->resourceID;
					break;
				}
			}
		}
		break; }
	case ComponentType::CAMERA: {
		ComponentCamera* camera = (ComponentCamera*)component;
		CompCameraZ* cameraZ = (CompCameraZ*)compZ;
		camera->camera_color_background = cameraZ->camera_color_background;
		camera->near_plane = cameraZ->near_plane;
		camera->far_plane = cameraZ->far_plane;
		camera->horizontal_fov = cameraZ->horizontal_fov;
		camera->vertical_fov = cameraZ->vertical_fov;
		camera->is_fov_horizontal = cameraZ->is_fov_horizontal;
		// set frustum
		camera->frustum.verticalFov = camera->vertical_fov * Maths::Deg2Rad();
		camera->frustum.horizontalFov = camera->horizontal_fov * Maths::Deg2Rad();
		camera->frustum.nearPlaneDistance = camera->near_plane;
		camera->frustum.farPlaneDistance = camera->far_plane;
		break; }

	case ComponentType::PARTICLES: {
		ComponentParticleSystem* ps = (ComponentParticleSystem*)component;
		CompParticleSystemZ* psZ = (CompParticleSystemZ*)compZ;
		ps->particleSystem->emmitter.SetMaxLife(psZ->duration);

		break; }
	}

	component->SetEnable(compZ->enabled);
	component->ID = compZ->compID;
}

void CompZ::AttachCompZToGameObject(CompZ* compZ)
{
	GameObject* obj = App->objects->GetGameObjectByID(compZ->objectID);
	if (obj == nullptr)
		return;

	switch (compZ->type) {
	case ComponentType::MESH: {
		ComponentMesh* mesh = new ComponentMesh(obj);
		CompZ::SetComponent(mesh, compZ);
		obj->AddComponent(mesh);
		break; }
	case ComponentType::MATERIAL: {
		ComponentMaterial* material = new ComponentMaterial(obj);
		CompZ::SetComponent(material, compZ);
		obj->AddComponent(material);
		break; }
	case ComponentType::LIGHT_DIRECTIONAL: {
		ComponentLightDirectional* light = new ComponentLightDirectional(obj);
		CompZ::SetComponent(light, compZ);
		obj->AddComponent(light);
		break; }
	case ComponentType::LIGHT_SPOT: {
		ComponentLightSpot* light = new ComponentLightSpot(obj);
		CompZ::SetComponent(light, compZ);
		obj->AddComponent(light);
		break; }
	case ComponentType::LIGHT_POINT: {
		ComponentLightPoint* light = new ComponentLightPoint(obj);
		CompZ::SetComponent(light, compZ);
		obj->AddComponent(light);
		break; }
	case ComponentType::CANVAS: {
		ComponentCanvas* canvas = new ComponentCanvas(obj);
		CompZ::SetComponent(canvas, compZ);
		obj->AddComponent(canvas);
		break; }
	case ComponentType::CAMERA: {
		ComponentCamera* camera = new ComponentCamera(obj);
		CompZ::SetComponent(camera, compZ);
		obj->AddComponent(camera);
		break; }
	case ComponentType::UI: {
		switch (compZ->ui_type) {
		case ComponentType::UI_IMAGE: {
			ComponentImage* image = new ComponentImage(obj);
			CompZ::SetComponent(image, compZ);
			obj->AddComponent(image);
			break; }
		}
		break; }
	case ComponentType::SCRIPT: {
		ComponentScript* script = new ComponentScript(obj);
		CompZ::SetComponent(script, compZ);
		break; }
	}
}

ActionDeleteObject::~ActionDeleteObject()
{
	if (object != nullptr)
		delete object;
}

ObjZ::~ObjZ()
{
	if (!children.empty()) {
		std::vector< ActionDeleteObject*>::iterator item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr) {
				delete* item;
				*item = nullptr;
			}
		}
	}
	if (!comps.empty()) {
		std::vector<CompZ*>::iterator item = comps.begin();
		for (; item != comps.end(); ++item) {
			if (*item != nullptr) {
				delete* item;
				*item = nullptr;
			}
		}
	}
}
