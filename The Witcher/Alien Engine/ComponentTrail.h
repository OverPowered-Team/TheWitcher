#pragma once


#include "Component.h"
#include "Color.h"
class JSONArraypack;
class ParticleSystem;
class ResourceTexture;
class Trail;

class __declspec(dllexport) ComponentTrail : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleObjects;
	friend class PanelCreateObject;
	friend class ModuleImporter;

public:

	ComponentTrail(GameObject* parent);
	~ComponentTrail();

private:

	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;

	void DrawScene() override;
	void DrawGame() override;

	void DebugDraw();
	void Draw();

	void OnEnable() override;
	void OnDisable() override;

	bool DrawInspector() override;

	

	// Serialization Component
	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

public:
	void Start();
	void Stop();
	math::OBB GetOBBFromObject();
	Trail* GetTrail();

private:
	Trail* trail = nullptr;
	int bbTypeSelected = 2;
	
};