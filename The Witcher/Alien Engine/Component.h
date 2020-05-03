#pragma once

#include <string>

class GameObject;
struct AlienEvent;

typedef unsigned int uint;
class JSONArraypack;
typedef unsigned long long u64;

enum class ComponentType {
	NONE = -1,
	TRANSFORM = 0,
	MESH = 1,
	MATERIAL = 2,
	LIGHT_DIRECTIONAL = 3,
	LIGHT_SPOT = 4,
	LIGHT_POINT = 5,
	CAMERA = 6,
	BOX_COLLIDER = 7,
	SPHERE_COLLIDER = 8,
	CAPSULE_COLLIDER = 9,
	CONVEX_HULL_COLLIDER = 10,
	RIGID_BODY = 11,
	POINT_CONSTRAINT = 12,
	CHARACTER_CONTROLLER = 13,
	ANIMATOR = 14,
	PARTICLES = 15,
	A_EMITTER = 16,
	A_LISTENER = 17,

	CANVAS = 19,
	UI_IMAGE = 20,
	UI_BUTTON = 21,
	UI_TEXT = 22,
	UI_CHECKBOX = 23,
	UI_SLIDER = 24,
	UI_BAR = 25,
	UI_ANIMATED_IMAGE = 26,
	DEFORMABLE_MESH = 27,
	BONE = 28,
	SCRIPT = 29,
	CURVE = 30,

	UI, // SECOND LAST LAST LAST
	MAX //LAST LAST LAST
};

class __declspec(dllexport) Component {
	friend class ComponentCamera;
	friend class ComponentLightDirectional;
	friend class ComponentLightSpot;
	friend class ComponentLightPoint;
	friend class ComponentMaterial;
	friend class ComponentTransform;
	friend class ComponentMesh;
	friend class ComponentDeformableMesh;
	friend class ComponentMaterial;
	friend class ComponentCanvas;
	friend class ComponentUI;
	friend class ComponentPhysics;
	friend class ComponentCharacterController;
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
	friend class ModulePhysX;
	friend class ModuleUI;
public:
	Component(GameObject* attach);
	virtual ~Component();

	bool IsEnabled();
	void SetEnable(bool enable);
	void Destroy();

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

	virtual void DrawScene(ComponentCamera* camera) {}
	virtual void DrawGame(ComponentCamera* camera) {}

	virtual void ResetIDs();

	const ComponentType& GetType() const;

	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void HandleAlienEvent(const AlienEvent& alien_event) {};

	virtual bool DrawInspector() { return true; }

	static std::string EnumToString(ComponentType type);

protected:

	void RightClickMenu(const char* collapsing_header_name);

protected:

	ComponentType type = ComponentType::NONE;
	bool enabled = true;
	u64 ID = 0;
	bool not_destroy = true;
	const char* name = "";
	bool serialize = true;
};
