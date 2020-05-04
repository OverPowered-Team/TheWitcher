#include "Color.h"
#include "GameObject.h"
#include "glew/include/glew.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentDeformableMesh.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentLightPoint.h"
#include "ComponentBone.h"
#include "ComponentAnimator.h"
#include "ComponentDeformableMesh.h"
#include "ComponentCanvas.h"
#include "ComponentText.h"
#include "ComponentButton.h"
#include "ComponentCurve.h"
#include "RandomHelper.h"
#include "ModuleObjects.h"
#include "ComponentCamera.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ComponentImage.h"
#include "ComponentBar.h"
#include "ComponentUI.h"
#include "ComponentCheckbox.h"
#include "ComponentSlider.h"
#include "ComponentAnimatedImage.h"
#include "ComponentScript.h"
#include "ComponentAudioListener.h"
#include "ComponentAudioEmitter.h"
#include "Prefab.h"
#include "Event.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "ResourcePrefab.h"
#include "ReturnZ.h"
#include "mmgr/mmgr.h"

#include "ResourceShader.h"
#include "ResourceMaterial.h"

#include "ComponentBoxCollider.h"
#include "ComponentSphereCollider.h"
#include "ModuleUI.h"
#include "PanelScene.h"
#include "Alien.h"
#include "ComponentCapsuleCollider.h"
#include "ComponentConvexHullCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentCharacterController.h"

#include "Optick/include/optick.h"

GameObject::GameObject(GameObject* parent, bool ignore_transform)
{
	ID = App->resources->GetRandomID();
	
	if (!ignore_transform) {
		this->transform = new ComponentTransform(this, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 });
		AddComponent(transform);
	}

	if (parent != nullptr) {
		this->parent = parent;
		parent->AddChild(this);
	}

}

GameObject::GameObject(GameObject* parent, const float3& pos, const Quat& rot, const float3& scale)
{
	ID = App->resources->GetRandomID();
	this->transform = new ComponentTransform(this, pos, rot, scale);
	AddComponent(transform);

	if (parent != nullptr) {
		this->parent = parent;
		parent->AddChild(this);
	}

}

GameObject::GameObject(bool ignore_transform)
{
	if (!ignore_transform) {
		this->transform = new ComponentTransform(this, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 });
		AddComponent(transform);
	}
}

GameObject::~GameObject()
{
	if (std::find(App->objects->GetSelectedObjects().begin(), App->objects->GetSelectedObjects().end(), this) != App->objects->GetSelectedObjects().end()) {
		App->objects->DeselectObject(this);
		App->ui->panel_scene->gizmo_curve = false;
		App->ui->panel_scene->curve = nullptr;
		App->ui->panel_scene->curve_index = 0;
	}

	App->objects->octree.Remove(this);

	std::vector<Component*>::iterator component = components.begin();

	while (component != components.end())
	{
		delete* component;
		component = components.erase(component);
	}

	std::vector<GameObject*>::iterator child = children.begin();
	
	while (child != children.end())
	{
		delete* child;
		child = children.erase(child);
	}
}

GameObject* GameObject::GetChild(const char* child_name)
{
	auto item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr && App->StringCmp((*item)->name, child_name)) {
			return (*item);
		}
	}
	return nullptr;
}

GameObject* GameObject::GetChild(const int& index)
{
	if (index < children.size() && children[index] != nullptr) {
		return children[index];
	}
	return nullptr;
}

GameObject* GameObject::GetChildRecursive(const char* child_name)
{
	auto item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			if (App->StringCmp((*item)->name, child_name)) {
				return (*item);
			}
			(*item)->GetChildRecursive(child_name);
		}
	}
}

std::vector<GameObject*>& GameObject::GetChildren()
{
	return children;
}

void GameObject::SetEnable(bool enable)
{
	if (enable != enabled) {
		enabled = enable;
		if (enable) {
			OnEnable();
		}
		else {
			OnDisable();
		}
		SayChildrenParentIsEnabled(enabled);
	}
}

bool GameObject::IsEnabled() const
{
	return enabled;
}

void GameObject::PreDrawScene(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position)
{
	OPTICK_EVENT();
	ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh == nullptr) //not sure if this is the best solution
		mesh = (ComponentMesh*)GetComponent(ComponentType::DEFORMABLE_MESH);


	if (mesh != nullptr && mesh->IsEnabled())
	{
		if (material == nullptr || (material != nullptr && !material->IsEnabled())) // set the basic color if the GameObject hasn't a material
			glColor3f(1, 1, 1);
		if (!mesh->wireframe)
			mesh->PreDrawPolygonForShadows(camera, ViewMat, ProjMatrix, position);
	}
}

void GameObject::DrawScene(ComponentCamera* camera, const float4& clip_plane)
{
	OPTICK_EVENT();

	for (Component* component : components)
	{
		component->DrawScene(camera);
	}
}

void GameObject::PreDrawGame(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position)
{
	OPTICK_EVENT();
	ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);

	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
	if (mesh == nullptr) //not sure if this is the best solution
		mesh = (ComponentMesh*)GetComponent(ComponentType::DEFORMABLE_MESH);

	/*if (material != nullptr && material->IsEnabled() && mesh != nullptr && mesh->IsEnabled())
	{
		material->BindTexture();
	}*/

	if (mesh != nullptr && mesh->IsEnabled())
	{
		if (material == nullptr || (material != nullptr && !material->IsEnabled())) // set the basic color if the GameObject hasn't a material
			glColor3f(1, 1, 1);
		mesh->PreDrawPolygonForShadows(camera, ViewMat, ProjMatrix, position);

	}
}

void GameObject::DrawGame(ComponentCamera* camera, const float4& clip_plane)
{
	OPTICK_EVENT();

	for (Component* component : components)
	{
		component->DrawGame(camera);
	}

}

void GameObject::SetDrawList(std::vector<std::pair<float, GameObject*>>* to_draw, std::vector<std::pair<float, GameObject*>>* to_draw_ui, const ComponentCamera* camera)
{
	OPTICK_EVENT();
	// TODO: HUGE TODO!: REVIEW THIS FUNCTION 
	if (!is_static) {
		ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
		if (mesh == nullptr) //not sure if this is the best solution
			mesh = (ComponentMesh*)GetComponent(ComponentType::DEFORMABLE_MESH);

		if (mesh != nullptr && mesh->mesh != nullptr) {
			if (App->renderer3D->IsInsideFrustum(camera, mesh->GetGlobalAABB())) {
				float3 obj_pos = transform->GetGlobalPosition();
				float distance = camera->frustum.pos.Distance(obj_pos);
				to_draw->push_back({ distance, this });
			}
		}
		else
		{
			float3 obj_pos = transform->GetGlobalPosition();
			float distance = camera->frustum.pos.Distance(obj_pos);
			to_draw->push_back({ distance, this });
		}
	}

	std::vector<GameObject*>::iterator child = children.begin();
	for (; child != children.end(); ++child) {
		if (*child != nullptr && (*child)->IsEnabled()) {
			(*child)->SetDrawList(to_draw, to_draw_ui, camera);
		}
	}

	ComponentUI* ui = GetComponent<ComponentUI>();

	if (ui != nullptr && ui->IsEnabled())
	{
		float3 obj_pos = transform->GetGlobalPosition();
		float distance = obj_pos.z;
		//ui->Draw(!App->objects->printing_scene);
		to_draw_ui->push_back({ distance, this });
	}
}

ComponentCanvas* GameObject::GetCanvas()
{
	ComponentCanvas* canvas = GetComponent<ComponentCanvas>();

	if (canvas != nullptr) {
		return canvas;
	}

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			canvas = (*item)->GetCanvas();
			if (canvas != nullptr)
				break;
		}
	}

	return canvas;
}

void GameObject::AddComponent(Component* component)
{
	for (auto item = components.begin(); item != components.end(); ++item) {
		if (*item != nullptr && *item == component) {
			return;
		}
	}
	components.push_back(component);
}

void GameObject::PostUpdate()
{
	OPTICK_EVENT();
	if (!components.empty()) {
		auto item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr) {
				(*item)->PostUpdate();
			}
		}
	}

	if (!children.empty()) {
		auto item = children.begin();
		for (; item != children.end(); ++item) {
			(*item)->PostUpdate();
		}
	}
}

void GameObject::PreUpdate()
{
	OPTICK_EVENT();
	if (!components.empty()) {
		auto item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr) {
				(*item)->PreUpdate();
			}
		}
	}

	if (!children.empty()) {
		auto item = children.begin();
		for (; item != children.end(); ++item) {
			(*item)->PreUpdate();
		}
	}
}

void GameObject::Update()
{
	OPTICK_EVENT();
	if (!components.empty()) {
		auto item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr) {
				(*item)->Update();
			}
		}
	}

	if (!children.empty()) {
		auto item = children.begin();
		for (; item != children.end(); ++item) {
			(*item)->Update();
		}
	}
}

bool GameObject::HasComponent(ComponentType component) const
{
	bool exists = false;

	if (components.size()== 0)
		return exists;

	std::vector<Component*>::const_iterator item = components.cbegin();
	for (; item != components.cend(); ++item) {
		if (*item != nullptr && (*item)->GetType() == component)
		{
			exists = true;
			break;
		}
	}

	return exists;
}

void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
}

void GameObject::SetName(const char* name)
{
	strcpy(this->name, name);
}

const char* GameObject::GetName() const
{
	return name;
}

const char* GameObject::ToString()
{
	return name;
}

void GameObject::SetTag(const char* tag)
{
	strcpy(this->tag, tag);
}

const char* GameObject::GetTag() const
{
	return tag;
}

Component* GameObject::GetComponent(const ComponentType& type)
{
	OPTICK_EVENT();

	if (type == ComponentType::UI_BUTTON || type == ComponentType::UI_IMAGE || type == ComponentType::UI_CHECKBOX || type == ComponentType::UI_BAR || type == ComponentType::UI_SLIDER || type == ComponentType::UI_ANIMATED_IMAGE || type == ComponentType::UI_TEXT) {
		std::vector<Component*>::iterator item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr && (*item)->GetType() == ComponentType::UI && dynamic_cast<ComponentUI*>(*item)->ui_type == type) {
				return *item;
			}
		}
	}
	else {
		std::vector<Component*>::iterator item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr && (*item)->GetType() == type) {
				return *item;
			}
		}
	}
	return nullptr;
}

ComponentTransform* GameObject::GetComponentTransform()
{
	auto ret = GetComponent(ComponentType::TRANSFORM);
	return (ret == nullptr) ? nullptr : (ComponentTransform*)ret;
}

Component* GameObject::GetComponentWithID(const u64& compID)
{
	std::vector<Component*>::iterator item = components.begin();
	for (; item != components.end(); ++item) {
		if (*item != nullptr && (*item)->ID == compID) {
			return *item;
		}
	}
	return nullptr;
}

const Component* GameObject::GetComponentWithID(const u64& compID) const
{
	std::vector<Component*>::const_iterator item = components.cbegin();
	for (; item != components.cend(); ++item) {
		if (*item != nullptr && (*item)->ID == compID) {
			return *item;
		}
	}
	return nullptr;
}

void GameObject::RemoveComponent(Component* component)
{
	std::vector<Component*>::iterator item = components.begin();
	for (; item != components.end(); ++item) {
		if (*item != nullptr && *item == component) {
			delete* item;
			(*item) = nullptr;
			components.erase(item);
			break;
		}
	}
}

bool GameObject::IsSelected() const
{
	return selected;
}

bool GameObject::IsParentSelected() const
{
	return parent_selected;
}

void GameObject::ChangeSelected(const bool& select)
{
	selected = select;

	SayChildrenParentIsSelected(selected);
}

void GameObject::ChangeWireframe(const bool& wireframe)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->wireframe = wireframe;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeWireframe(wireframe);
		}
	}
}

void GameObject::ChangeMeshView(const bool& mesh_view)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->view_mesh = mesh_view;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeMeshView(mesh_view);
		}
	}
}

void GameObject::ChangeVertexNormalsView(const bool& normals)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->view_vertex_normals = normals;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeVertexNormalsView(normals);
		}
	}
}

void GameObject::ChangeFaceNormalsView(const bool& normals)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->view_face_normals = normals;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeFaceNormalsView(normals);
		}
	}
}

void GameObject::ChangeAABB(const bool& AABB_view)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->draw_AABB = AABB_view;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeAABB(AABB_view);
		}
	}
}

void GameObject::ChangeOBB(const bool& OBB_view)
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh != nullptr)
		mesh->draw_OBB = OBB_view;

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->ChangeOBB(OBB_view);
		}
	}
}

bool GameObject::HasChildren() const
{
	return !children.empty();
}

void GameObject::SetNewParent(GameObject* new_parent)
{
	if (new_parent != nullptr && !Exists(new_parent)) {
		parent->children.erase(std::find(parent->children.begin(), parent->children.end(), this));
		parent = new_parent;
		parent->AddChild(this);

		ComponentTransform* parent_transform = (ComponentTransform*)parent->GetComponent(ComponentType::TRANSFORM);
		ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);

		if (parent_transform != nullptr) {
			transform->Reparent(parent_transform->global_transformation.Inverted() * transform->global_transformation);
		}
		else {
			transform->Reparent(transform->global_transformation);
		}

		ComponentUI* ui = GetComponent<ComponentUI>();
		if (ui != nullptr) {
			GameObject* p = new_parent;
			bool changed = true;
			while (changed) {
				if (p != nullptr) {
					ComponentCanvas* canvas = p->GetComponent <ComponentCanvas>();
					if (canvas != nullptr) {
						ui->SetCanvas(canvas);
						changed = false;
					}
					p = p->parent;
				}
				else {
					changed = false;
					ui->SetCanvas(nullptr);
				}
			}
		}
	}
	else {
		LOG_ENGINE("NewParent was nullptr or NewParent was a child :O");
	}
}

void GameObject::ToDelete()
{
	to_delete = true;
	App->objects->need_to_delete_objects = true;
	App->CastEvent(EventType::ON_GO_DELETE);
#ifndef GAME_VERSION
	if (!App->objects->in_cntrl_Z) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::DELETE_OBJECT, this);
	}
#endif
}

void GameObject::SayChildrenParentIsEnabled(const bool& enabled)
{
	if (enabled)
		OnEnable();
	else
		OnDisable();

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->parent_enabled = enabled;
			(*item)->SayChildrenParentIsEnabled(enabled);
		}
	}
}

bool GameObject::IsParentEnabled() const
{
	return parent_enabled;
}

void GameObject::Destroy(GameObject* object)
{
	object->ToDelete();
}

void GameObject::DestroyInstantly(GameObject* object)
{
	if (object->parent != nullptr) {
		auto item = object->parent->children.begin();
		for (; item != object->parent->children.end(); ++item) {
			if (*item == object) {
				object->parent->children.erase(item);
				break;
			}
		}
	}
	delete object;
}

GameObject* GameObject::FindWithName(const char* name)
{
	return App->objects->GetRoot(true)->Find(name);
}

GameObject* GameObject::FindWithTag(const char* tag_to_find)
{
	return App->objects->GetRoot(true)->FindTag(tag_to_find);
}

std::vector<GameObject*>& GameObject::FindGameObjectsWithTag(const char* tag_to_find)
{
	static std::vector<GameObject*> found;
	found.clear();
	App->objects->GetRoot(true)->FindTags(tag_to_find, &found);
	return found;
}

GameObject* GameObject::Instantiate(const Prefab& prefab, const float3& position, bool check_child, GameObject* parent)
{
	OPTICK_EVENT();
	if (prefab.prefabID != 0) {
		ResourcePrefab* r_prefab = (ResourcePrefab*)App->resources->GetResourceWithID(prefab.prefabID);
		if (r_prefab != nullptr && App->StringCmp(prefab.prefab_name.data(), r_prefab->GetName())) {
			r_prefab->ConvertToGameObjects((parent == nullptr) ? App->objects->GetRoot(true) : parent, -1, position, check_child, false);
			return (parent == nullptr) ? App->objects->GetRoot(true)->children.back() : parent->children.back();
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
}

GameObject* GameObject::Instantiate(const char* prefab, const float3& position, bool check_child, GameObject* parent)
{
	OPTICK_EVENT();
	for (auto item = App->resources->resources.begin(); item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_PREFAB && strcmp(prefab, (*item)->GetName()) == 0) {
			dynamic_cast<ResourcePrefab*>(*item)->ConvertToGameObjects((parent == nullptr) ? App->objects->GetRoot(true) : parent, -1, position, check_child, false);
			return (parent == nullptr) ? App->objects->GetRoot(true)->children.back() : parent->children.back();
		}
	}
	return nullptr;
}

GameObject* GameObject::CloneObject(GameObject* to_clone, GameObject* parent)
{
	GameObject* clone = new GameObject((parent == nullptr) ? to_clone->parent : parent);
	to_clone->CloningGameObject(clone);
	return clone;
}

void GameObject::OnEnable()
{
	std::vector<Component*>::iterator item = components.begin();
	for (; item != components.end(); ++item) {
		if (*item != nullptr) {
			(*item)->OnEnable();
		}
	}

	std::vector<GameObject*>::iterator it = children.begin();
	for (; it != children.end(); ++it) {
		if (*it != nullptr) {
			(*it)->OnEnable();
		}
	}
}

void GameObject::OnDisable()
{
	std::vector<Component*>::iterator item = components.begin();
	for (; item != components.end(); ++item) {
		if (*item != nullptr) {
			(*item)->OnDisable();
		}
	}

	std::vector<GameObject*>::iterator it = children.begin();
	for (; it != children.end(); ++it) {
		if (*it != nullptr) {
			(*it)->OnDisable();
		}
	}
}

void GameObject::OnPlay()
{
	std::vector<Component*>::const_iterator it = components.begin();

	for (it; it != components.end(); ++it)
	{
		if ((*it)->IsEnabled())
			(*it)->OnPlay();
	}
}

void GameObject::OnPause()
{
	std::vector<Component*>::const_iterator it = components.begin();

	for (it; it != components.end(); ++it)
	{
		if ((*it)->IsEnabled())
			(*it)->OnPause();
	}
}

void GameObject::OnStop()
{
	std::vector<Component*>::const_iterator it = components.begin();

	for (it; it != components.end(); ++it)
	{
		if ((*it)->IsEnabled())
			(*it)->OnStop();
	}
}

void GameObject::ScaleNegative(const bool& is_negative)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			ComponentTransform* tr = (ComponentTransform*)(*item)->GetComponent(ComponentType::TRANSFORM);
			if (tr != nullptr)
				tr->SetScaleNegative(is_negative);
			(*item)->ScaleNegative(is_negative);
		}
	}
}

bool GameObject::IsUpWardsEnabled() const
{
	GameObject* to_look = parent;
	while (to_look != nullptr) {
		if (!to_look->enabled) {
			return false;
		}
		to_look = to_look->parent;
	}
	return true;
}

GameObject* GameObject::Find(const char* name)
{
	GameObject* ret = nullptr;
	if (App->StringCmp(name, this->name)) {
		return this;
	}
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			ret = (*item)->Find(name);
			if (ret != nullptr)
				break;
		}
	}
	return ret;
}

void GameObject::SayChildrenParentIsSelected(const bool& selected)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->parent_selected = selected;
			(*item)->SayChildrenParentIsSelected(selected);
		}
	}
}

void GameObject::ReTag(const char* from, const char* to)
{
	if (App->StringCmp(tag, from)) {
		strcpy(tag, to);
	}
	for (uint i = 0; i < children.size(); ++i) {
		if (children[i] != nullptr) {
			children[i]->ReTag(from, to);
		}
	}
}

void GameObject::SendAlienEventHierarchy(void* object, AlienEventType type)
{
	AlienEvent alien_event;
	alien_event.object = object;
	alien_event.type = type;
	std::stack<GameObject*> go_stack;
	go_stack.push(this);

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();
		go_stack.pop();

		for (Component* component : go->components)
		{
			if (component)
				component->HandleAlienEvent(alien_event);
		}

		for (GameObject* child : go->children)
		{
			go_stack.push(child);
		}
	}
}

void GameObject::SendAlienEventAll(void* object, AlienEventType type)
{
	AlienEvent alien_event;
	alien_event.object = object;
	alien_event.type = type;
	App->objects->HandleAlienEvent(alien_event);
}

void GameObject::SendAlientEventThis(void* object, AlienEventType type)
{
	AlienEvent alien_event;
	alien_event.object = object;
	alien_event.type = type;

	for (Component* component : components)
	{
		if (component)
			component->HandleAlienEvent(alien_event);
	}
}


GameObject* GameObject::GetGameObjectByID(const u64 & id)
{
	GameObject* ret = nullptr;
	if (id == this->ID) {
		return this;
	}
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			ret = (*item)->GetGameObjectByID(id);
			if (ret != nullptr)
				break;
		}
	}
	return ret;
}

GameObject* GameObject::GetGameObjectByIDReverse(const u64& id)
{
	GameObject* ret = nullptr;
	if (id == this->ID) {
		return this;
	}
	auto item = children.rbegin();
	for (; item != children.rend(); ++item) {
		if (*item != nullptr) {
			ret = (*item)->GetGameObjectByID(id);
			if (ret != nullptr)
				break;
		}
	}
	return ret;
}

GameObject* GameObject::FindTag(const char* tag_to_find)
{
	GameObject* ret = nullptr;
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			if (App->StringCmp((*item)->tag, tag_to_find)) {
				return (*item);
			}
			ret = (*item)->FindTag(tag_to_find);
			if (ret != nullptr)
				break;
		}
	}
	return ret;
}

void GameObject::FindTags(const char* tag_to_find, std::vector<GameObject*>* objects)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			if (App->StringCmp((*item)->tag, tag_to_find)) {
				objects->push_back((*item));
			}
			(*item)->FindTags(tag_to_find, objects);
		}
	}
}

bool GameObject::Exists(GameObject* object) const
{
	bool ret = false;

	if (this == object)
		return true;

	std::vector<GameObject*>::const_iterator item = children.cbegin();
	for (; item != children.cend(); ++item) {
		if (*item != nullptr) {
			ret = (*item)->Exists(object);
			if (ret)
				break;
		}
	}

	return ret;
}

AABB GameObject::GetBB()
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
	if (mesh == nullptr)
		mesh = (ComponentMesh*)GetComponent(ComponentType::DEFORMABLE_MESH);

	if (HasChildren())
	{
		AABB parent_aabb;
		if (mesh != nullptr)
		{
			parent_aabb = mesh->GetGlobalAABB();
		}
		else
		{
			parent_aabb.SetNegativeInfinity();
		}

		for (std::vector<GameObject*>::const_iterator iter = children.cbegin(); iter != children.cend(); ++iter)
		{
			AABB child_aabb = (*iter)->GetBB();
			parent_aabb.maxPoint = parent_aabb.maxPoint.Max(child_aabb.maxPoint);
			parent_aabb.minPoint = parent_aabb.minPoint.Min(child_aabb.minPoint);
		}

		return parent_aabb;
	}

	else
	{
		if (mesh != nullptr)
		{
			return mesh->GetGlobalAABB();
		}
		else
		{
			ComponentUI* ui = (ComponentUI*)GetComponent(ComponentType::UI);
			ComponentCurve* curve = GetComponent<ComponentCurve>();
			if (ui != nullptr) {
				AABB aabb_ui;
				ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);
				float3 pos = transform->GetGlobalPosition();
				float3 scale = transform->GetGlobalScale();
				aabb_ui.SetFromCenterAndSize(pos, { scale.x * 2,scale.y * 2,2 });
				return aabb_ui;
			}
			else if (curve != nullptr) {
				AABB aabb;
				aabb.SetNegativeInfinity();
				for (uint i = 0; i < curve->curve.GetControlPoints().size(); ++i) {
					aabb.maxPoint = float3(max(aabb.maxPoint.x, curve->curve.GetControlPoints()[i].x), max(aabb.maxPoint.y, curve->curve.GetControlPoints()[i].y), max(aabb.maxPoint.z, curve->curve.GetControlPoints()[i].z));
					aabb.minPoint = float3(min(aabb.minPoint.x, curve->curve.GetControlPoints()[i].x), min(aabb.minPoint.y, curve->curve.GetControlPoints()[i].y), min(aabb.minPoint.z, curve->curve.GetControlPoints()[i].z));
				}
				return aabb;
			}
			

			AABB aabb_null;
			ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);
			float3 pos = transform->GetGlobalPosition();
			aabb_null.SetFromCenterAndSize(pos, { 1,1,1 });
			return aabb_null;
		}
	}
}

OBB GameObject::GetGlobalOBB()
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);

	if (HasChildren())
	{
		OBB parent_obb = GetBB();
		parent_obb.Transform(transform->global_transformation);
		return parent_obb;
	}

	else
	{
		if (mesh != nullptr)
		{
			return mesh->GetOBB();
		}
		else
		{
			AABB aabb_null;
			ComponentTransform* transform = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);
			float3 pos = transform->GetGlobalPosition();
			aabb_null.SetFromCenterAndSize(pos, { 2,2,2 });
			return aabb_null;
		}
	}
}

void GameObject::SaveObject(JSONArraypack* to_save, const uint& family_number)
{
	OPTICK_EVENT();
	to_save->SetString("Name", name);
	to_save->SetNumber("FamilyNumber", family_number);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetString("ParentID",(parent != nullptr) ? std::to_string(parent->ID).data() : "0");
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetBoolean("ParentEnabled", parent_enabled);
	to_save->SetBoolean("Selected", selected);
	to_save->SetBoolean("ParentSelected", parent_selected);
	to_save->SetBoolean("IsStatic", is_static);
	to_save->SetBoolean("IsPrefab", IsPrefab());
	to_save->SetBoolean("PrefabLocked", prefab_locked);
	to_save->SetString("Tag", tag);
	if (IsPrefab()) {
		to_save->SetString("PrefabID", std::to_string(prefabID).data());
	}

	JSONArraypack* components_to_save = to_save->InitNewArray("Components");

	std::vector<Component*>::iterator item = components.begin();
	for (; item != components.end(); ++item) {
		if (*item != nullptr && (*item)->serialize) {
			(*item)->SaveComponent(components_to_save);
			if ((*item) != components.back())
				components_to_save->SetAnotherNode();
		}
	}
}

void GameObject::LoadObject(JSONArraypack* to_load, GameObject* parent, bool force_no_selected)
{
	strcpy(name, to_load->GetString("Name"));
	ID = std::stoull(to_load->GetString("ID"));
	enabled = to_load->GetBoolean("Enabled");
	parent_enabled = to_load->GetBoolean("ParentEnabled");
	if (!force_no_selected && to_load->GetBoolean("Selected") && !App->objects->inPrefabCreation) {
		App->objects->SetNewSelectedObject(this, false);
	}
	prefab_locked = to_load->GetBoolean("PrefabLocked");
	parent_selected = to_load->GetBoolean("ParentSelected");
	is_static = to_load->GetBoolean("IsStatic");
	std::string tag_ = to_load->GetString("Tag");
	if (std::find(App->objects->tags.begin(), App->objects->tags.end(), tag_) != App->objects->tags.end()) {
		strcpy(tag, tag_.data());
	}
	if (to_load->GetBoolean("IsPrefab")) {
		u64 id = std::stoull(to_load->GetString("PrefabID"));
		if (App->resources->GetResourceWithID(id) != nullptr) {
			prefabID = id;
		}
	}
	if (parent != nullptr) {
		this->parent = parent;
		parent->AddChild(this);
	}

	JSONArraypack* components_to_load = to_load->GetArray("Components");

	if (components_to_load != nullptr) {
		for (uint i = 0; i < components_to_load->GetArraySize(); ++i) {
			switch ((int)components_to_load->GetNumber("Type")) {
			case (int)ComponentType::TRANSFORM: {
				transform = new ComponentTransform(this);
				transform->LoadComponent(components_to_load);
				AddComponent(transform);
				break; }
			case (int)ComponentType::LIGHT_DIRECTIONAL: {
				ComponentLightDirectional* light = new ComponentLightDirectional(this);
				light->LoadComponent(components_to_load);
				AddComponent(light);
				break; }
			case (int)ComponentType::LIGHT_SPOT: {
				ComponentLightSpot* light = new ComponentLightSpot(this);
				light->LoadComponent(components_to_load);
				AddComponent(light);
				break; }
			case (int)ComponentType::LIGHT_POINT: {
				ComponentLightPoint* light = new ComponentLightPoint(this);
				light->LoadComponent(components_to_load);
				AddComponent(light);
				break; }
			case (int)ComponentType::MATERIAL: {
				ComponentMaterial* material = new ComponentMaterial(this);
				material->LoadComponent(components_to_load);
				AddComponent(material);
				break; }
			case (int)ComponentType::MESH: {
				ComponentMesh* mesh = new ComponentMesh(this);
				mesh->LoadComponent(components_to_load);
				AddComponent(mesh);
				break; }
			case (int)ComponentType::CAMERA: {
				ComponentCamera* camera = new ComponentCamera(this);
				camera->LoadComponent(components_to_load);
				AddComponent(camera);
				break; }
			case (int)ComponentType::DEFORMABLE_MESH: {
				ComponentDeformableMesh* def_mesh = new ComponentDeformableMesh(this);
				def_mesh->LoadComponent(components_to_load);
				AddComponent(def_mesh);
				break; }
			case (int)ComponentType::ANIMATOR: {
				ComponentAnimator* anim = new ComponentAnimator(this);
				anim->LoadComponent(components_to_load);
				AddComponent(anim);
				break; }
			case (int)ComponentType::BONE: {
				ComponentBone* bone = new ComponentBone(this);
				bone->LoadComponent(components_to_load);
				AddComponent(bone);
				break; }
			case (int)ComponentType::A_EMITTER: {
				ComponentAudioEmitter* emitter = new ComponentAudioEmitter(this);
				emitter->LoadComponent(components_to_load);
				AddComponent(emitter);
				break; }
			case (int)ComponentType::A_LISTENER: {
				ComponentAudioListener* listener = new ComponentAudioListener(this);
				listener->LoadComponent(components_to_load);
				AddComponent(listener);
				break; }
			case (int)ComponentType::CURVE: {
				ComponentCurve* curve = new ComponentCurve(this);
				curve->LoadComponent(components_to_load);
				AddComponent(curve);
				break; }
			case (int)ComponentType::PARTICLES: {
				ComponentParticleSystem* particleSystem = new ComponentParticleSystem(this);
				particleSystem->LoadComponent(components_to_load);
				AddComponent(particleSystem);
				break; }
			case (int)ComponentType::CANVAS: {
				ComponentCanvas* canvas = new ComponentCanvas(this);
				canvas->LoadComponent(components_to_load);
				AddComponent(canvas);
				break; }
			case (int)ComponentType::BOX_COLLIDER: {
				ComponentBoxCollider* box_collider = new ComponentBoxCollider(this);
				box_collider->LoadComponent(components_to_load);
				AddComponent(box_collider);
				break; }
			case (int)ComponentType::SPHERE_COLLIDER: {
				ComponentSphereCollider* sphere_collider = new ComponentSphereCollider(this);
				sphere_collider->LoadComponent(components_to_load);
				AddComponent(sphere_collider);
				break; }
			case (int)ComponentType::CAPSULE_COLLIDER: {
				ComponentCapsuleCollider* capsule_collider = new ComponentCapsuleCollider(this);
				capsule_collider->LoadComponent(components_to_load);
				AddComponent(capsule_collider);
				break; }
			case (int)ComponentType::CONVEX_HULL_COLLIDER: {
				ComponentConvexHullCollider* convex_hull_collider = new ComponentConvexHullCollider(this);
				convex_hull_collider->LoadComponent(components_to_load);
				AddComponent(convex_hull_collider);
				break; }
			case (int)ComponentType::RIGID_BODY: {
				ComponentRigidBody* rigi_body = new ComponentRigidBody(this);
				rigi_body->LoadComponent(components_to_load);
				AddComponent(rigi_body);
				break; }
			case (int)ComponentType::CHARACTER_CONTROLLER: {
				ComponentCharacterController* character_controller = new ComponentCharacterController(this);
				character_controller->LoadComponent(components_to_load);
				AddComponent(character_controller);
				break; }
			case (int)ComponentType::SCRIPT: {
				ComponentScript* script = new ComponentScript(this);
				script->LoadComponent(components_to_load);
				// dont need to addcomponent, load script does it
				break; }
			case (int)ComponentType::UI: {
				ComponentType typeUI = (ComponentType)(int)components_to_load->GetNumber("UIType");
				switch (typeUI) {
				case ComponentType::UI_IMAGE: {
					ComponentImage* image = new ComponentImage(this);
					image->ui_type = typeUI;
					image->LoadComponent(components_to_load);
					AddComponent(image);
					break; }
				case ComponentType::UI_TEXT: {
					ComponentText* text = new ComponentText(this);
					text->ui_type = typeUI;
					text->LoadComponent(components_to_load);
					AddComponent(text);
					break; }
				case ComponentType::UI_BUTTON: {
					ComponentButton* button = new ComponentButton(this);
					button->ui_type = typeUI;
					button->LoadComponent(components_to_load);
					AddComponent(button);
					break; }
				case ComponentType::UI_BAR: {
					ComponentBar* bar = new ComponentBar(this);
					bar->ui_type = typeUI;
					bar->LoadComponent(components_to_load);
					AddComponent(bar);
					break; }
				case ComponentType::UI_CHECKBOX: {
					ComponentCheckbox* checkbox = new ComponentCheckbox(this);
					checkbox->ui_type = typeUI;
					checkbox->LoadComponent(components_to_load);
					AddComponent(checkbox);
					break; }
				case ComponentType::UI_SLIDER: {
					ComponentSlider* slider = new ComponentSlider(this);
					slider->ui_type = typeUI;
					slider->LoadComponent(components_to_load);
					AddComponent(slider);
					break; }
				case ComponentType::UI_ANIMATED_IMAGE: {
					ComponentAnimatedImage* aImage = new ComponentAnimatedImage(this);
					aImage->ui_type = typeUI;
					aImage->LoadComponent(components_to_load);
					AddComponent(aImage);
					break; }
				default:
					LOG_ENGINE("Unknown component UItype while loading");
					break;
				}
				break; }
			default:
				LOG_ENGINE("Unknown component type while loading");
				break;
			}

			components_to_load->GetAnotherNode();
		}
	}

	if (is_static) {
		App->objects->octree.Insert(this, false);
	}

}

GameObject* GameObject::Clone(GameObject* parent)
{
	OPTICK_EVENT();
	GameObject* clone = new GameObject((parent == nullptr) ? this->parent : parent, true);
	CloningGameObject(clone);
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, clone);
	return clone;
}

void GameObject::CloningGameObject(GameObject* clone)
{
	clone->to_delete = to_delete;
	clone->prefabID = prefabID;

	std::string name_ = name;
	if (name_.back() != ')') {
		strcpy(clone->name, std::string(name + std::string(" (1)")).data());
	}
	else {
		int num = std::stoi(&(name_.at(name_.size() - 2)));
		int offset = std::to_string(num).size() + 2;
		std::string nam(name_.begin(), name_.size() - offset + name_.begin());
		nam += std::string("(" + std::to_string(num + 1) + std::string(")"));
		strcpy(clone->name, nam.data());
	}
	
	strcpy(clone->tag, tag);
	clone->enabled = enabled;
	clone->parent_enabled = parent_enabled;
	clone->prefab_locked = prefab_locked;
	clone->is_static = is_static;

	if (!components.empty()) {
		auto item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr) {
				switch ((*item)->GetType()) {
				case ComponentType::TRANSFORM: {
					ComponentTransform* trans = new ComponentTransform(clone);
					(*item)->Clone(trans);
					clone->AddComponent(trans);
					clone->transform = trans;
					break; }
				case ComponentType::LIGHT_DIRECTIONAL: {
					ComponentLightDirectional* light = new ComponentLightDirectional(clone);
					(*item)->Clone(light);
					clone->AddComponent(light);
					break; }
				case ComponentType::LIGHT_SPOT: {
					ComponentLightSpot* light = new ComponentLightSpot(clone);
					(*item)->Clone(light);
					clone->AddComponent(light);
					break; }
				case ComponentType::LIGHT_POINT: {
					ComponentLightPoint* light = new ComponentLightPoint(clone);
					(*item)->Clone(light);
					clone->AddComponent(light);
					break; }
				case ComponentType::MATERIAL: {
					ComponentMaterial* material = new ComponentMaterial(clone);
					(*item)->Clone(material);
					clone->AddComponent(material);
					break; }
				case ComponentType::CANVAS: {
					ComponentCanvas* canvas = new ComponentCanvas(clone);
					(*item)->Clone(canvas);
					clone->AddComponent(canvas);
					break; }
				case ComponentType::MESH: {
					ComponentMesh* mesh = new ComponentMesh(clone);
					(*item)->Clone(mesh);
					clone->AddComponent(mesh);
					break; }
				case ComponentType::CAMERA: {
					ComponentCamera* camera = new ComponentCamera(clone);
					(*item)->Clone(camera);
					clone->AddComponent(camera);
					break; }
				case ComponentType::DEFORMABLE_MESH: {
					ComponentDeformableMesh* def_mesh = new ComponentDeformableMesh(clone);
					(*item)->Clone(def_mesh);
					clone->AddComponent(def_mesh);
					break; }
				case ComponentType::SCRIPT: {
					ComponentScript* script = new ComponentScript(clone);
					(*item)->Clone(script);
					// dont need to addcomponent, clone script does it
					break; }
				case ComponentType::UI: {
					ComponentUI* ui = (ComponentUI*)GetComponent(ComponentType::UI);
					switch (ui->ui_type) {
					case ComponentType::UI_IMAGE: {
						ComponentImage* image = new ComponentImage(clone);
						(*item)->Clone(image);
						clone->AddComponent(image);
						break; }
					case ComponentType::UI_TEXT: {
						ComponentText* text = new ComponentText(clone);
						(*item)->Clone(text);
						clone->AddComponent(text);
						break; }
					case ComponentType::UI_BUTTON: {
						ComponentButton* button = new ComponentButton(clone);
						(*item)->Clone(button);
						clone->AddComponent(button);
						break; }
					case ComponentType::UI_CHECKBOX: {
						ComponentCheckbox* check = new ComponentCheckbox(clone);
						(*item)->Clone(check);
						clone->AddComponent(check);
						break; }
					case ComponentType::UI_SLIDER: {
						ComponentSlider* slider = new ComponentSlider(clone);
						(*item)->Clone(slider);
						clone->AddComponent(slider);
						break; }
					case ComponentType::UI_BAR: {
						ComponentBar* bar = new ComponentBar(clone);
						(*item)->Clone(bar);
						clone->AddComponent(bar);
						break; }
					case ComponentType::UI_ANIMATED_IMAGE: {
						ComponentAnimatedImage* aImage = new ComponentAnimatedImage(clone);
						(*item)->Clone(aImage);
						clone->AddComponent(aImage);
						break; }
					}
					break; }
				case ComponentType::BOX_COLLIDER: {
					ComponentBoxCollider* collider = new ComponentBoxCollider(clone);
					(*item)->Clone(collider);
					clone->AddComponent(collider);
					break; }
				case ComponentType::SPHERE_COLLIDER: {
					ComponentSphereCollider* collider = new ComponentSphereCollider(clone);
					(*item)->Clone(collider);
					clone->AddComponent(collider);
					break; }
				case ComponentType::CAPSULE_COLLIDER: {
					ComponentCapsuleCollider* collider = new ComponentCapsuleCollider(clone);
					(*item)->Clone(collider);
					clone->AddComponent(collider);
					break; }
				case ComponentType::CONVEX_HULL_COLLIDER: {
					ComponentConvexHullCollider* collider = new ComponentConvexHullCollider(clone);
					(*item)->Clone(collider);
					clone->AddComponent(collider);
					break; }
				case ComponentType::RIGID_BODY: {
					ComponentRigidBody* rb = new ComponentRigidBody(clone);
					(*item)->Clone(rb);
					clone->AddComponent(rb);
					break; }
				default:
					LOG_ENGINE("Unknown component type while loading");
					break;
				}
			}
		}
	}

	if (clone->is_static) {
		App->objects->octree.Insert(clone, false);
	}

	if (!children.empty()) {
		auto item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr) {
				GameObject* child = new GameObject(clone, true);
				(*item)->CloningGameObject(child);
			}
		}
	}
	
}

void GameObject::SearchResourceToDelete(const ResourceType& type, Resource* to_delete)
{
	//SDL_assert((uint)FileDropType::UNKNOWN == 10);
	switch (type) {
	case ResourceType::RESOURCE_TEXTURE: {
		ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);
		if (material != nullptr ) {
			for (uint texType = 0; texType < (uint)TextureType::MAX; ++texType)
			{
				const ResourceTexture* texture = material->GetTexture((TextureType)texType);
				if(texture != nullptr && texture == (ResourceTexture*)to_delete)
					material->RemoveTexture((TextureType)texType);
			}
		}
		break; }
	case ResourceType::RESOURCE_MATERIAL : {
		ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);
		if (material != nullptr && material->material == (ResourceMaterial*)to_delete) {
			material->material = App->resources->default_material; // TODO: Apply default material when deleting the old one
		}
		break; }
	case ResourceType::RESOURCE_SHADER: {
		ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);
		if (material != nullptr && material->material->used_shader == (ResourceShader*)to_delete) {
			material->material->used_shader = App->resources->default_shader;
			App->resources->default_shader->IncreaseReferences();
		}
		break; }
	case ResourceType::RESOURCE_MESH: {
		ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
		if (!mesh)
		{
			ComponentDeformableMesh* d_mesh = (ComponentDeformableMesh*)GetComponent(ComponentType::DEFORMABLE_MESH);
			if (d_mesh != nullptr && d_mesh->mesh == (ResourceMesh*)to_delete) {
				d_mesh->mesh = nullptr;
			}
		}
		else
		{
			if (mesh != nullptr && mesh->mesh == (ResourceMesh*)to_delete) {
				mesh->mesh = nullptr;
			}
		}
		break; }
	case ResourceType::RESOURCE_BONE: {
		ComponentBone* bone = (ComponentBone*)GetComponent(ComponentType::BONE);
		if (bone != nullptr && bone->bone == (ResourceBone*)to_delete) {
			bone->bone = nullptr;
		}
		break; }
	case ResourceType::RESOURCE_ANIMATOR_CONTROLLER: {
		ComponentAnimator* anim = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
		if (anim != nullptr && anim->animator_controller == (ResourceAnimatorController*)to_delete) {
			anim->animator_controller = nullptr;
		}
		break; }
	case ResourceType::RESOURCE_FONT: {
		ComponentText* text = (ComponentText*)GetComponent(ComponentType::UI_TEXT);
		if (text != nullptr && text->GetFont() == (ResourceFont*)to_delete) {
			text->SetFont(nullptr);
		}
		break; }
	}

	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->SearchResourceToDelete(type, to_delete);
		}
	}
}

void GameObject::SetPrefab(const u64& prefabID)
{
	if (!children.empty()) {
		std::vector<GameObject*>::iterator item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr && (prefabID != 0 || (*item)->prefabID == this->prefabID)) {
				(*item)->SetPrefab(prefabID);
			}
		}
	}
	this->prefabID = prefabID;
}

void GameObject::UnpackPrefab()
{
	if (!IsPrefab())
		return;

	SetPrefab(0);
}

void GameObject::UnpackAllPrefabsOf(const u64& prefabID)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			if ((*item)->prefabID == prefabID) {
				(*item)->prefabID = 0;
			}
			(*item)->UnpackAllPrefabsOf(prefabID);
		}
	}
}

void GameObject::GetComponentsChildren(const ComponentType& type, std::vector<Component*>* to_fill, bool recursive)
{
	auto item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			auto item2 = (*item)->components.begin();
			for (; item2 != (*item)->components.end(); ++item2) {
				if (*item2 != nullptr && (*item2)->GetType() == type) {
					to_fill->push_back(*item2);
				}
			}
			if (recursive) {
				(*item)->GetComponentsChildren(type, to_fill, true);
			}
		}
	}
}

GameObject* GameObject::FindPrefabRoot()
{
	if (!IsPrefab())
		return nullptr;

	if (parent->IsPrefab()) {
		GameObject* find_root = this;
		for (;;) {
			if (find_root->parent != nullptr && (!find_root->parent->IsPrefab() || find_root->parent->prefabID != find_root->prefabID)) {
				return find_root;
			}
			find_root = find_root->parent;
		}
	}
	else {
		return this;
	}
}

bool GameObject::IsPrefab() const
{
	return prefabID != 0;
}

const u64 GameObject::GetPrefabID() const
{
	return prefabID;
}

void GameObject::GetObjectWithPrefabID(const u64& prefID, std::vector<GameObject*>* objs)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			if ((*item)->IsPrefab() && (*item)->prefabID == prefID && (*item)->FindPrefabRoot() == (*item)) {
				objs->push_back((*item));
			}
			else
				(*item)->GetObjectWithPrefabID(prefID, objs);
		}
	}
}

void GameObject::LockPrefab(bool lock)
{
	if (!IsPrefab())
		return;

	prefab_locked = lock;
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->LockPrefab(lock);
		}
	}
}

void GameObject::GetAllPrefabRoots(std::vector<GameObject*>& roots)
{
	if (IsPrefab() && FindPrefabRoot() == this) {
		roots.push_back(this);
	}
	else {
		auto item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr) {
				(*item)->GetAllPrefabRoots(roots);
			}
		}
	}
}

void GameObject::ResetIDs()
{
	ID = App->resources->GetRandomID();

	if (!components.empty()) {
		std::vector<Component*>::iterator item = components.begin();
		for (; item != components.end(); ++item) {
			if (*item != nullptr) {
				(*item)->ResetIDs();
			}
		}
	}
	if (!children.empty()) {
		std::vector<GameObject*>::iterator item = children.begin();
		for (; item != children.end(); ++item) {
			if (*item != nullptr) {
				(*item)->ResetIDs();
			}
		}
	}
}

void GameObject::ChangeStatic(bool static_)
{
	std::vector<GameObject*>::iterator item = children.begin();
	for (; item != children.end(); ++item) {
		if (*item != nullptr) {
			(*item)->is_static = static_;
			(*item)->ChangeStatic(static_);
		}
	}
}

bool GameObject::HasChildrenStatic() const 
{
	bool ret = false;

	if (!children.empty()) {
		std::vector<GameObject*>::const_iterator item = children.cbegin();
		for (; item != children.cend(); ++item) {
			if (*item != nullptr) {
				if (ret)
					break;

				if ((*item)->is_static) {
					ret = true;
					break;
				}
				else {
					ret = (*item)->HasChildrenStatic();
				}
			}
		}

	}
	return ret;
}

void GameObject::SearchToDelete()
{
	std::vector<GameObject*>::iterator item = children.begin();
	while (item != children.end()) {

		if ((*item)->to_delete) {
			delete* item;
			*item = nullptr;
			item = children.erase(item);
		}
		else {

			std::vector<Component*>::iterator item_com = (*item)->components.begin();
			while (item_com != (*item)->components.end())
			{
				if ((*item_com) != nullptr && (*item_com)->not_destroy != true)
				{
					delete* item_com;
					*item_com = nullptr;
					item_com = (*item)->components.erase(item_com);
					
				}
				else
				{
					++item_com;
				}
			}
			(*item)->SearchToDelete();
			++item;
		}
	}
}