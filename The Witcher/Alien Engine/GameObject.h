#pragma once

#include "Globals.h"
#include "Component.h"
#include "ComponentScript.h"
#include <vector>
#include <string>
#include <map>
#include <stack>
#include "MathGeoLib/include/MathGeoLib.h"
#include "JSONfilepack.h"
#include "Alien.h"
#include "Event.h"

enum class ResourceType;
class Resource;
class Prefab;
class ComponentCanvas;
class ComponentCamera;
class Alien;
class ComponentTransform;

class __declspec(dllexport) GameObject
{
	friend class Component;
	friend class ComponentCamera;
	friend class ComponentParticleSystem;
	friend class ComponentLightDirectional;
	friend class ComponentLightSpot;
	friend class ComponentLightPoint;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentMesh;
	friend class ComponentDeformableMesh;
	friend class ComponentBone;
	friend class ComponentAnimator;
	friend class ComponentMaterial;
	friend class ComponentScript;
	friend class ComponentUI;
	friend class ComponentCanvas;
	friend class ComponentCurve;
	friend class ComponentCheckbox;
	friend class Component;
	friend class ComponentText;
	friend class ComponentButton;
	friend class ComponentBar;
	friend class ComponentAnimatedImage;
	friend class ComponentImage;
	friend class ComponentSlider;

	friend class ComponentBasePhysic;
	friend class ComponentPhysics;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentPointConstraint;
	friend class ComponentRigidBody;
	friend class ComponentCharacterController;

	friend class PanelHierarchy;
	friend class PanelAnimTimeline;
	friend class PanelScene;
	friend class PanelAnimator;
	friend class PanelParticleSystem;
	friend class PanelCreateObject;
	friend class PanelInspector;

	friend class ResourceModel;
	friend class ResourceMesh;
	friend class ResourcePrefab;
	friend class PanelSceneSelector;
	friend class ResourceTexture;
	friend class ResourceAnimatorController;

	friend class ModulePhysX;
	friend class ModuleCamera3D;
	friend class ModuleObjects;
	friend class ModuleTween;
	friend class ModuleImporter;
	friend class ModuleRenderer3D;
	friend class ModuleUI;

	friend class GameObject;
	friend class Prefab;
	friend class Octree;
	friend class OctreeNode;
	friend class FileNode;
	friend class AnimTween;
	friend class FadeToBlack;
	friend class Viewport;
	friend class ReturnZ;
	friend class CompZ;

public:
	GameObject(GameObject* parent, bool ignore_transform = false);
	GameObject(GameObject* parent, const float3& pos, const Quat& rot, const float3& scale);
	GameObject(bool ignore_transform = false); // just for loading objects, dont use it
	virtual ~GameObject();

public:

	static void Destroy(GameObject* object);
	static void DestroyInstantly(GameObject* object);
	static GameObject* FindWithName(const char* name);
	static GameObject* FindWithTag(const char* tag_to_find);
	// return the sie of the array of gameobjects found, pass a GameObject** nullptr with &. Remember to delete it with GameObject::FreeArrayMemory!!!
	static std::vector<GameObject*>& FindGameObjectsWithTag(const char* tag_to_find);
	// parent = nullptr is root
	static GameObject* Instantiate(const Prefab& prefab, const float3& position, bool check_child = false, GameObject* parent = nullptr);
	static GameObject* Instantiate(const char* prefab, const float3& position, bool check_child = false, GameObject* parent = nullptr);
	static GameObject* CloneObject(GameObject* to_clone, GameObject* parent = nullptr);
	// TODO:
	/*
		change static, is static... cant move in code if is static!!

		DontDestroyOnLoad();
	*/
	template<class Comp>
	bool HasComponent();

	template <class Comp>
	Comp* GetComponent();
	template <class Comp>
	std::vector<Comp*> GetComponents();

	template <class Comp>
	Comp* GetComponentInParent();
	template <class Comp>
	std::vector<Comp*> GetComponentsInParent();

	template <class Comp>
	Comp* GetComponentInChildren();
	template <class Comp>
	std::vector<Comp*> GetComponentsInChildren();

	template <class Comp>
	Comp* GetComponentInChildrenRecursive();
	template <class Comp>
	std::vector<Comp*> GetComponentsInChildrenRecursive();




	GameObject* GetChild(const char* child_name);
	GameObject* GetChild(const int& index);
	// look for child of child of child bla bla
	GameObject* GetChildRecursive(const char* child_name);
	std::vector<GameObject*>& GetChildren();

	void SetEnable(bool enable);
	bool IsEnabled() const;

	ComponentTransform* GetComponentTransform(); //sorry ori

	// parent
	void SetNewParent(GameObject* new_parent);

	// components
	bool HasComponent(ComponentType component) const;

	// children
	bool HasChildren() const;

	// GameObject name
	void SetName(const char* name);
	const char* GetName() const;
	const char* ToString();

	// GameObject tag
	void SetTag(const char* tag);
	const char* GetTag() const;

	// delete it at the begining of the next frame
	void ToDelete(); 

private:
	Component* GetComponent(const ComponentType& type);

	// OnEnable/Disable
	void OnEnable();
	void OnDisable();

	void OnPlay();
	void OnPause();
	void OnStop();

	// here we call Component Mesh, Material & light
	void PreDrawGame(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position);
	void PreDrawScene(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position);
	void DrawGame(ComponentCamera* camera, const float4& clip_plane);
	void DrawScene(ComponentCamera* camera, const float4& clip_plane);
	void SetDrawList(std::vector<std::pair<float, GameObject*>>* to_draw, std::vector<std::pair<float, GameObject*>>* to_draw_ui, const ComponentCamera* camera);

	ComponentCanvas* GetCanvas();

	Component* GetComponentWithID(const u64& ID);
	const Component* GetComponentWithID(const u64& ID) const;
	void RemoveComponent(Component* component);
	void AddComponent(Component* component);

	void PreUpdate();
	void Update();
	void PostUpdate();

	void AddChild(GameObject* child);

	// selected object
	bool IsSelected() const;
	bool IsParentSelected() const;
	void ChangeSelected(const bool& select);

	// enabling
	void SayChildrenParentIsEnabled(const bool& enabled);
	bool IsParentEnabled() const;

	// flip poly if scale negative
	void ScaleNegative(const bool& is_negative);

	bool IsUpWardsEnabled() const;

	// render poly options
	void ChangeWireframe(const bool& wireframe);
	void ChangeMeshView(const bool& wireframe);
	void ChangeVertexNormalsView(const bool& wireframe);
	void ChangeFaceNormalsView(const bool& wireframe);
	void ChangeAABB(const bool& AABB_view);
	void ChangeOBB(const bool& OBB_view);

	// deleting 
	void SearchToDelete();

	// search and return true if exists in its children or children of children bla bla
	bool Exists(GameObject* object) const;

	// Bounding Boxes
	AABB GetBB(); // 0 = Local Bounding Box, 1 = Global Bounding Box
	OBB GetGlobalOBB();
	void SaveObject(JSONArraypack* to_save, const uint& family_number);
	void LoadObject(JSONArraypack* to_save, GameObject* parent, bool force_no_selected = false);

	GameObject* Clone(GameObject* parent = nullptr);
	void CloningGameObject(GameObject* clone);

	void SearchResourceToDelete(const ResourceType& type, Resource* to_delete);

	// prefab
	void SetPrefab(const u64& prefabID);
	void UnpackPrefab();
	void UnpackAllPrefabsOf(const u64& prefabID);
	GameObject* FindPrefabRoot();
	bool IsPrefab() const;
	const u64 GetPrefabID() const;
	void GetObjectWithPrefabID(const u64& prefabID, std::vector<GameObject*>* objs);
	void LockPrefab(bool lock);
	void GetAllPrefabRoots(std::vector<GameObject*>& roots);

	void ResetIDs();

	//static
	void ChangeStatic(bool static_);
	bool HasChildrenStatic() const;

	// find
	GameObject* Find(const char* name);
	GameObject* GetGameObjectByID(const u64& id);
	GameObject* GetGameObjectByIDReverse(const u64& id);
	GameObject* FindTag(const char* tag_to_find);
	void FindTags(const char* tag_to_find, std::vector<GameObject*>* objects);

	// parent selected
	void SayChildrenParentIsSelected(const bool& selected);
	void GetComponentsChildren(const ComponentType& type, std::vector<Component*>* to_fill, bool recursive);
	void ReTag(const char* from, const char* to);


	// Events 

	void SendAlienEventHierarchy(void* object, AlienEventType type); // Only hierarchy component
	void SendAlienEventAll(void* object, AlienEventType type);   // All scene components
	void SendAlientEventThis(void* object, AlienEventType type); // Itself components

public:

	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;

private:
	bool to_delete = false; 
	u64 prefabID = 0;

	bool selected = false;

	std::vector<Component*> components;
	std::vector<GameObject*> children;

	char name[MAX_PATH] = "UnNamed";
	char tag[MAX_PATH] = "UnTagged";

	bool enabled = true;
	bool is_static = false;
	u64 ID = 0;
	bool parent_enabled = true;
	bool parent_selected = false;
	bool open_node = false;
	bool prefab_locked = false;
};

template<class Comp>
inline Comp* GameObject::GetComponent()
{
	for (uint i = 0; i < components.size(); ++i) {
		if (components[i]->GetType() != ComponentType::SCRIPT) {
			Comp* component = dynamic_cast<Comp*>(components[i]);
			if (component != nullptr) {
				return component;
			}
		}
		else {
			Comp* component = dynamic_cast<Comp*>(components[i]);
			if (component != nullptr) {
				return component;
			}
			ComponentScript* script = (ComponentScript*)components[i];
			if (script->need_alien) {
				Alien* alien = (Alien*)script->data_ptr;
				Comp* component = dynamic_cast<Comp*>(alien);
				if (component != nullptr) {
					return component;
				}
			}
		}
	}
	return nullptr;
}

template<class Comp>
inline std::vector<Comp*> GameObject::GetComponents()
{
	std::vector<Comp*> comps;
	for (uint i = 0; i < components.size(); ++i) {
		if (components[i]->GetType() != ComponentType::SCRIPT) {
			Comp* component = dynamic_cast<Comp*>(components[i]);
			if (component != nullptr) {
				comps.push_back(component);
			}
		}
		else {
			Comp* component = dynamic_cast<Comp*>(components[i]);
			if (component != nullptr) {
				comps.push_back(component);
			}
			ComponentScript* script = (ComponentScript*)components[i];
			if (script->need_alien) {
				Alien* alien = (Alien*)script->data_ptr;
				Comp* component = dynamic_cast<Comp*>(alien);
				if (component != nullptr) {
					comps.push_back(component);
				}
			}
		}

	}
	return comps;
}

template<class Comp>
inline Comp* GameObject::GetComponentInParent()
{
	return parent->GetComponent<Comp>();
}

template<class Comp>
inline std::vector<Comp*> GameObject::GetComponentsInParent()
{
	return parent->GetComponents<Comp>();
}

template<class Comp>
inline Comp* GameObject::GetComponentInChildren()
{
	for (auto item = children.begin(); item != children.end(); ++item) {
		for (auto it = (*item)->components.begin(); it != (*item)->components.end(); ++it) {
			if ((*it)->GetType() != ComponentType::SCRIPT) {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					return component;
				}
			}
			else {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					return component;
				}
				ComponentScript* script = (ComponentScript*)(*it);
				if (script->need_alien) {
					Alien* alien = (Alien*)script->data_ptr;
					Comp* component = dynamic_cast<Comp*>(alien);
					if (component != nullptr) {
						return component;
					}
				}
			}
		}
	}
	return nullptr;
}

template<class Comp>
inline std::vector<Comp*> GameObject::GetComponentsInChildren()
{
	std::vector<Comp*> comps;
	for (auto item = children.begin(); item != children.end(); ++item) {
		for (auto it = (*item)->components.begin(); it != (*item)->components.end(); ++it) {
			if ((*it)->GetType() != ComponentType::SCRIPT) {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					comps.push_back(component);
				}
			}
			else {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					comps.push_back(component);
				}
				ComponentScript* script = (ComponentScript*)(*it);
				if (script->need_alien) {
					Alien* alien = (Alien*)script->data_ptr;
					Comp* component = dynamic_cast<Comp*>(alien);
					if (component != nullptr) {
						comps.push_back(component);
					}
				}
			}
		}
	}
	return comps;
}

template<class Comp>
inline Comp* GameObject::GetComponentInChildrenRecursive()
{
	std::stack<GameObject*> objects;
	for (auto item = children.begin(); item != children.end(); ++item) {
		objects.push(*item);
	}
	
	while (!objects.empty()) {
		GameObject* obj = objects.top();
		objects.pop();

		for (auto it = obj->components.begin(); it != obj->components.end(); ++it) {
			if ((*it)->GetType() != ComponentType::SCRIPT) {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					return component;
				}
			}
			else {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					return component;
				}
				ComponentScript* script = (ComponentScript*)(*it);
				if (script->need_alien) {
					Alien* alien = (Alien*)script->data_ptr;
					Comp* component = dynamic_cast<Comp*>(alien);
					if (component != nullptr) {
						return component;
					}
				}
			}
		}

		for (auto item = obj->children.begin(); item != obj->children.end(); ++item) {
			objects.push(*item);
		}
	}
	return nullptr;
}

template<class Comp>
inline std::vector<Comp*> GameObject::GetComponentsInChildrenRecursive()
{
	std::vector<Comp*> comps;
	std::stack<GameObject*> objects;
	for (auto item = children.begin(); item != children.end(); ++item) {
		objects.push(*item);
	}

	while (!objects.empty()) {
		GameObject* obj = objects.top();
		objects.pop();

		for (auto it = obj->components.begin(); it != obj->components.end(); ++it) {
			if ((*it)->GetType() != ComponentType::SCRIPT) {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					comps.push_back(component);
				}
			}
			else {
				Comp* component = dynamic_cast<Comp*>((*it));
				if (component != nullptr) {
					comps.push_back(component);
				}
				ComponentScript* script = (ComponentScript*)(*it);
				if (script->need_alien) {
					Alien* alien = (Alien*)script->data_ptr;
					Comp* component = dynamic_cast<Comp*>(alien);
					if (component != nullptr) {
						comps.push_back(component);
					}
				}
			}
		}

		for (auto item = obj->children.begin(); item != obj->children.end(); ++item) {
			objects.push(*item);
		}
	}
	return comps;
}

template<class Comp>
inline bool GameObject::HasComponent()
{
	for (uint i = 0; i < components.size(); ++i) {
		Comp* component = dynamic_cast<Comp*>(components[i]);
		if (component != nullptr) {
			return true;
		}
	}
	return false;
}

