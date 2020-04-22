#pragma once

#include <functional>
#include "ComponentScript.h"
#include "GameObject.h"

class ComponentTransform;
class ComponentCharacterController;
class Collision;
class ContactPoint;
class ControllerColliderHit;
class ComponentCollider;
class GameObject;

enum class ComponentType;
class Component;
typedef unsigned int uint;

class __declspec(dllexport) Alien {
	friend class ComponentScript;
	friend class ComponentCollider;
	friend class ModuleObjects;
	friend class ComponentTransform;
	friend class ResourceAnimatorController;
	friend class SimulationEventCallback;
	friend class ComponentCharacterController;
public:
	Alien();
	virtual ~Alien();

	virtual void Awake() {} 
	virtual void Start() {} 

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	virtual void OnDisable() {} 
	virtual void OnEnable() {} 

	virtual void OnDrawGizmos() {}
	virtual void OnDrawGizmosSelected() {}

	// called to the script attached before the camera start culling
	virtual void OnPreCull() {}
	// called to the script attached after a camera finishes rendering
	virtual void OnPreRender() {}
	// called to the script attached after a camera finishes rendering
	virtual void OnPostRender() {}

	virtual void CleanUp() {}


	// Physics ------------------------------------------------

	virtual void OnCollisionEnter(const Collision& collision) {}
	virtual void OnCollisionStay(const Collision& collision) {}
	virtual void OnCollisionExit(const Collision& collision) {}

	virtual void OnTriggerEnter(ComponentCollider* collider) {}
	virtual void OnTriggerStay(ComponentCollider* collider) {}
	virtual void OnTriggerExit(ComponentCollider* collider) {}

	// OnControllerColliderHit is called when the controller hits a collider while performing a Move.
	virtual void OnControllerColliderHit(const ControllerColliderHit& hit) {}

	// Deprecated , use OnTriggerStay()
	virtual void OnTrigger(ComponentCollider* collider) {}  // TODO:  Delete after change al code
	// --------------------------------------------------------

	virtual void OnAnimationEnd(const char* name) {}


	bool IsScriptEnabled() const;
	void SetScriptEnable(const bool& enable);

	const char* ToString();

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

	bool HasComponent(const ComponentType& component) const;

	static void Destroy(GameObject* obj);
	static void DestroyInstantly(GameObject* obj);

	bool CompareTag(const char* tag_to_compare) const;

	void Invoke(std::function<void()> void_no_params_function, float seconds);
	void CancelInvoke();
	void InvokeRepeating(std::function<void()> void_no_params_function, float seconds_to_first_invoke, float seconds_between_each_call);
	//bool IsInvoking(std::function<void()> void_no_params_function);



	// Need
	// TODO:
	/*
		change static, is static... cant move in code if is static!!

		DontDestroyOnLoad();
	*/


	// Extras
	// TODO: https://docs.unity3d.com/ScriptReference/MonoBehaviour.html
	/*
		OnBecameInvisible();
		OnBecameVisible();

		OnMouseDown();
		OnMouseUp();
		OnMouseDrag();
		OnMouseEnter();
		OnMouseExit();
		OnMouseOver();
	*/

public:

	ComponentTransform* transform = nullptr;
	GameObject* game_object = nullptr;

private:

	virtual bool IsAlien() { return true; }

	bool* enabled = nullptr;
	char data_name[260];
};

template<class Comp>
inline Comp* Alien::GetComponent()
{
	return game_object->GetComponent<Comp>();
}

template<class Comp>
inline std::vector<Comp*> Alien::GetComponents()
{
	return game_object->GetComponents<Comp>();
}

template<class Comp>
inline Comp* Alien::GetComponentInParent()
{
	return game_object->parent->GetComponent<Comp>();
}

template<class Comp>
inline std::vector<Comp*> Alien::GetComponentsInParent()
{
	return game_object->parent->GetComponents<Comp>();
}

template<class Comp>
inline Comp* Alien::GetComponentInChildren()
{
	return game_object->GetComponentInChildren<Comp>();
}

template<class Comp>
inline std::vector<Comp*> Alien::GetComponentsInChildren()
{
	return game_object->GetComponentsInChildren<Comp>();
}

template<class Comp>
inline Comp* Alien::GetComponentInChildrenRecursive()
{
	return game_object->GetComponentInChildrenRecursive<Comp>();
}

template<class Comp>
inline std::vector<Comp*> Alien::GetComponentsInChildrenRecursive()
{
	return game_object->GetComponentsInChildrenRecursive<Comp>();
}
