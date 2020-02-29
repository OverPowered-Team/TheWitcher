#pragma once
#include "Module.h"
#include "Globals.h"
#include "glew/include/glew.h"
#include "ModuleImporter.h"
#include "ComponentCamera.h"

#define MAX_LIGHTS 8

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


	ComponentCamera* GetCurrentMainCamera();

public:

	SDL_GLContext context;

public:

	ComponentCamera* scene_fake_camera = nullptr;
	ComponentCamera* actual_game_camera = nullptr;
	ComponentCamera* selected_game_camera = nullptr;

	// grid
	int length_grid = 20;
	Color grid_color{ 1,1,1 };
	float grid_spacing = 1.f;
	int line_grid_width = 1;
};