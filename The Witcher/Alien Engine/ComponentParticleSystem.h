#pragma once

#include "Component.h"
#include "Color.h"
class JSONArraypack;
class ParticleSystem;
class ResourceTexture;

class __declspec(dllexport) ComponentParticleSystem : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleObjects;
	friend class PanelCreateObject;
	friend class ModuleImporter;

public: 

	ComponentParticleSystem(GameObject* parent);
	~ComponentParticleSystem();

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

	bool DrawInspector () override;

	//void TextureBrowser();

	// Serialization Component
	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

	// Serialization File
	void LoadParticles();
	void SaveParticles();

	void SetConfigurationArrow();
	float CalculateRandomBetweenTwoConstants(float2 constants);

public: 

	ParticleSystem* GetSystem();
	//void SetTexture(ResourceTexture* tex);
	
	//void SetMaterial(ResourceMaterial* mat);

	void Play();
	void Pause();
	void Restart(); 

	void OnPlay() override;
	void OnPause() override;
	void OnStop() override;
	void OnEmitterPlay();
	void OnEmitterStop();

private:

	bool drawEmmitter = false;
	int shapeSelected = 0;
	int bbTypeSelected = 0;
	int meshTypeSelected = 8; // 8 - None mesh
	int matTypeSelected = 0;
	int eqTypeSelected = 0;
	int funcTypeSource = 6;
	int funcTypeDest = 7;
	int transformSelected = 0; // 0 Global - 1 Local
	int castLightSelected = 0; // 0 Emitter - 1 Particles - 2 Both

	// Pointers
	ParticleSystem* particleSystem = nullptr;
	//ResourceTexture* texture = nullptr;
	//ResourceTexture* selected_texture = nullptr;

	float3 c_ambient = float3::zero();
	float3 c_diffuse = float3::zero();
	float intensity = 0;
	float new_intensity = 0;

	// Logic
	//bool change_texture_menu = false;
	//bool texture_activated = true;

	bool enable_render = false;
	bool enable_anim = false;
	bool enable_light = false;
	float animSpeed = 0.1f;
	int texRows = 1;
	int texColumns = 1;
	int startFrame = 0;
	int endFrame = (texRows * texColumns) - 1;
	
	//Temporal stuff
	bool random_rot = false;
};