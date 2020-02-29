#pragma once

class GameObject;

typedef unsigned int uint;
class JSONArraypack;
typedef unsigned long long u64;

enum class ComponentType {
	TRANSFORM = 0,
	MESH,
	MATERIAL,
	LIGHT,
	CAMERA,
	BOX_COLLIDER,
	SPHERE_COLLIDER,
	CAPSULE_COLLIDER,
	CONVEX_HULL_COLLIDER,
	RIGID_BODY,
	ANIMATOR,
	PARTICLES,
	A_EMITTER,
	A_LISTENER,
	A_REVERB,
	CANVAS,
	UI_IMAGE,
	UI_BUTTON,
	UI_TEXT,
	UI_CHECKBOX,
	UI_SLIDER,
	UI_BAR, 
	DEFORMABLE_MESH,
	BONE,
	SCRIPT,
	UI,// UI MUST BE THE LAST
	UNKNOWN
};

class __declspec(dllexport) Component {
	friend class ComponentCamera;
	friend class ComponentLight;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentMesh;
	friend class ComponentDeformableMesh;
	friend class ComponentMaterial;
	friend class ComponentCanvas;
	friend class ComponentUI;
	friend class ComponentRigidBody;
	friend class ComponentCollider;
	friend class ComponentScript;
	friend class ComponentImage;
	friend class ComponentText;
	friend class GameObject;
	friend class ReturnZ;
	friend class Prefab;
	friend class CompZ;
	friend class PanelInspector;
	friend class ModuleObjects;
	friend class ModuleUI;
public:
	Component(GameObject* attach);
	virtual ~Component();

	bool IsEnabled();
	void SetEnable(bool enable);
public:
	GameObject* game_object_attached = nullptr;

protected:

	virtual void Reset() {}
	virtual void SetComponent(Component* component) {}
	virtual void Clone(Component* clone) {}

	virtual void SaveComponent(JSONArraypack* to_save) {}
	virtual void LoadComponent(JSONArraypack* to_load) {}

	virtual void OnPlay() {}
	virtual void OnPause() {}
	virtual void OnStop() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}
	virtual void DrawScene() {}
	virtual void DrawGame() {}

	void ResetIDs();

	const ComponentType& GetType() const;

	virtual void OnEnable() {}
	virtual void OnDisable() {}

	virtual bool DrawInspector() { return true; }

protected:

	void RightClickMenu(const char* collapsing_header_name);

protected:

	ComponentType type = ComponentType::UNKNOWN;
	bool enabled = true;
	u64 ID = 0;
	bool not_destroy = true;
	const char* name = "";
};
