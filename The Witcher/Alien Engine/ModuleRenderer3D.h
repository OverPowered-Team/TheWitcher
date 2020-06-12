#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleImporter.h"
#include "SDL/include/SDL_video.h"
#include "ComponentCamera.h"

#define MAX_LIGHTS 8
#define CIRCLE_SIDES 50.F
#define HALF_CIRCLE_SIDES 25.F
#define FULL_CIRCLE 360.F
#define HALF_CIRCLE 180.F

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void RenderGrid();
	void UpdateCameraMatrix(ComponentCamera* camera);
	bool IsInsideFrustum(const ComponentCamera* camera, const AABB& aabb);

	static void BeginDebugDraw(float4& color);
	static void EndDebugDraw();

	void GenerateScreenQuadVAO();
	void DestroyScreenQuadVAO();

	void RenderCircleAroundZ(const float& x, const float& y, const float& z, const float& radius, const float& line_width = 2.0f, const int& segments = 50);
	void RenderCircleAroundX(const float& x, const float& y, const float& z, const float& radius, const float& line_width = 2.0f, const int& segments = 50);

	void DebugDrawBox(const float4x4& transform, const float3& size, const float3& color = float3::one()) const;
	void DebugDrawSphere(const float4x4& transform, float radius, const float3& color = float3::one()) const;
	void DebugDrawCapsule(const float4x4& transform,  float radius, float height, const float3& color = float3::one()) const;
	void DebugDrawMesh(const float4x4& transform, float* vertices, uint num_vertices,  uint* indices, uint num_indices , const float3 & color = float3::one()) const;
	void PanelConfigOption();

	ComponentCamera* GetCurrentMainCamera();
	

public:

	SDL_GLContext context;
	ComponentCamera* scene_fake_camera = nullptr;
	ComponentCamera* actual_game_camera = nullptr;
	ComponentCamera* selected_game_camera = nullptr;

	// grid
	int length_grid = 20;
	Color grid_color{ 1,1,1 };
	float grid_spacing = 1.f;
	int line_grid_width = 1;
	float4 last_color = float4(0.f, 0.f, 0.f, 0.f);
	bool render_skybox = true;

	// Screen quad to render
	uint screen_quad_VAO = 0;
	uint screen_quad_VBO = 0;

};