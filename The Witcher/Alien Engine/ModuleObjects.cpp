#include "Globals.h"
#include "ModuleObjects.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "glew/include/glew.h"
#include "Application.h"
#include <algorithm>
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ModuleWindow.h"
#include "ResourceScene.h"
#include "ComponentMesh.h"
#include "ComponentUI.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentBar.h"
#include "ComponentButton.h"
#include "ComponentCheckbox.h"
#include "ComponentSlider.h"
#include "ComponentText.h"
#include "ComponentAnimatedImage.h"
#include "ComponentCollider.h"
#include "ComponentBoxCollider.h"
#include "ModuleResources.h"
#include "ComponentSphereCollider.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentLightPoint.h"
#include "ComponentMaterial.h"
#include "ComponentAudioEmitter.h"
#include "ModuleUI.h"
#include "ModuleCamera3D.h"
#include "ModuleFileSystem.h"
#include "ModuleAudio.h"
#include "ComponentParticleSystem.h"
#include "ReturnZ.h"
#include "Time.h"
#include "Prefab.h"
#include "ResourcePrefab.h"
#include "ComponentDeformableMesh.h"
#include "ModuleRenderer3D.h"
#include "ComponentScript.h"
#include "PanelHierarchy.h"
#include "PanelAnimTimeline.h"
#include "StaticInput.h"
#include "Gizmos.h"
#include "Viewport.h"
#include "Alien.h"
#include "Event.h"
#include "PanelProject.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "ComponentAnimator.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include "ResourceScript.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"
#include "ModuleFadeToBlack.h"
#include "SceneManager.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

ModuleObjects::ModuleObjects(bool start_enabled) :Module(start_enabled)
{
	name.assign("ModuleObject");
}

ModuleObjects::~ModuleObjects()
{
	DeleteReturns();

	RELEASE(wfbos);
}

bool ModuleObjects::Init()
{
	tags.push_back(std::string("UnTagged"));

	base_game_object = new GameObject();
	base_game_object->ID = 0;
	base_game_object->is_static = true;

	if (App->file_system->Exists(FILE_TAGS)) {
		JSON_Value* value = json_parse_file(FILE_TAGS);
		JSON_Object* object = json_value_get_object(value);

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* json_tags = new JSONfilepack(FILE_TAGS, object, value);
			JSONArraypack* curr_tags = json_tags->GetArray("Tags");
			for (uint i = 0; i < curr_tags->GetArraySize(); ++i) {
				tags.push_back(curr_tags->GetString("Tag"));
				curr_tags->GetAnotherNode();
			}
			delete json_tags;
		}
	}

	return true;
}

bool ModuleObjects::Start()
{
	OPTICK_EVENT();
	LOG_ENGINE("Starting Module Objects");
	bool ret = true;

	game_viewport = new Viewport(nullptr);

	wfbos = new WaterFrameBuffers();

#ifndef GAME_VERSION
	GameObject* scene_root = new GameObject();
	scene_root->ID = App->resources->GetRandomID();
	scene_root->is_static = true;
	scene_root->SetName("Untitled*");
	scene_root->parent = base_game_object;
	base_game_object->children.push_back(scene_root);
	scene_active = scene_root->ID;
	GameObject* camera = new GameObject(scene_root);
	camera->SetName("Main Camera");
	camera->AddComponent(new ComponentCamera(camera));

	GameObject* light = new GameObject(scene_root);
	light->SetName("Directional Light");
	light->AddComponent(new ComponentLightDirectional(light));
	light->transform->SetGlobalRotation(math::Quat::LookAt(float3::unitZ(), float3(-0.5f, -0.5f, 0.5f), float3::unitY(), float3::unitY()));



#else 
	JSON_Value* value = json_parse_file(BUILD_SETTINGS_PATH);
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(BUILD_SETTINGS_PATH, object, value);

		SDL_SetWindowTitle(App->window->window, meta->GetString("Build.GameName"));
		LoadScene(App->file_system->GetBaseFileName(meta->GetString("Build.FirstScene")).data());
		game_viewport->SetPos({ 0,0 });
		game_viewport->active = true;
		App->renderer3D->OnResize(App->window->width, App->window->height);
		Time::Play();

		delete meta;
	}
	else {
		ret = false;
	}
#endif

	return ret;
}

update_status ModuleObjects::PreUpdate(float dt)
{
	OPTICK_EVENT();

	if (!sceneNameToChange.empty()) {
		LoadScene(sceneNameToChange.data());
		sceneNameToChange.clear();
	}

	// delete objects
	if (need_to_delete_objects) {
		need_to_delete_objects = false;
		base_game_object->SearchToDelete();
	}

	// change parent
	if (!to_reparent.empty()) {
		std::vector< std::tuple<GameObject*, GameObject*, bool>>::iterator item = to_reparent.begin();
		for (; item != to_reparent.end(); ++item) {
			if (std::get<0>(*item) != nullptr && std::get<1>(*item) != nullptr) {
				if (std::get<2>(*item))
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::REPARENT_HIERARCHY, std::get<0>(*item));
				std::get<0>(*item)->SetNewParent(std::get<1>(*item));
			}
		}
		to_reparent.clear();
	}

	if (!invokes.empty()) {
		std::list<InvokeInfo*> invokes_list = invokes;
		auto item = invokes_list.begin();
		for (; item != invokes_list.end(); ++item) {
			if ((*item)->time_started + (*item)->time_to_wait < Time::time_since_start) {
				(*item)->function();
				if (!(*item)->is_repeating) {
					invokes.remove(*item);
					delete* item;
					*item = nullptr;
				}
				else {
					(*item)->time_started = Time::time_since_start;
					(*item)->time_to_wait = (*item)->time_between;
				}
			}
		}
	}
	base_game_object->PreUpdate();
	ScriptsPreUpdate();

#ifndef GAME_VERSION
	for (Viewport* viewport : viewports)
	{
		if (!viewport->active || !viewport->CanRender() || (App->renderer3D->selected_game_camera == nullptr) && viewport == App->camera->selected_viewport)
			continue;

		viewport->BeginViewport();
		printing_scene = (viewport == App->camera->scene_viewport) ? true : false;
		bool isGameCamera = (viewport == game_viewport) ? true : false;

		viewport->EndViewport();
	}
#endif

	return UPDATE_CONTINUE;
}

update_status ModuleObjects::Update(float dt)
{
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == Input::KEY_DOWN)
	{
		if (GetSelectedObjects().front())
		{
			App->renderer3D->actual_game_camera->WorldToScreenPoint(GetSelectedObjects().front()->transform->GetGlobalPosition());
		}
	}

	OPTICK_EVENT();
	base_game_object->Update();
	if (!functions_to_call.empty()) {
		for (auto item = functions_to_call.begin(); item != functions_to_call.end(); ++item) {
			try {
				(*item)();
			}
			catch (...)
			{
				LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS WHEN CALLING A FUNTION IN TIMELINE");
			}
		}
		functions_to_call.clear();
	}
	UpdateGamePadInput();
	ScriptsUpdate();

	return UPDATE_CONTINUE;
}

update_status ModuleObjects::PostUpdate(float dt)
{
	OPTICK_EVENT();
	base_game_object->PostUpdate();
	ScriptsPostUpdate();

#ifndef GAME_VERSION

	static bool light_view = false;
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		light_view = !light_view;
	for (Viewport* viewport : viewports) {
		if (!viewport->active || !viewport->CanRender() || (App->renderer3D->selected_game_camera == nullptr) && viewport == App->camera->selected_viewport)
			continue;

		current_viewport = viewport;
		viewport->BeginViewport();
		printing_scene = (viewport == App->camera->scene_viewport) ? true : false;
		bool isGameCamera = (viewport == game_viewport) ? true : false;

		viewport->BeginViewport();
		if (printing_scene)
		{
			if (draw_ray)
				DrawRay();

			if (allow_grid)
				App->renderer3D->RenderGrid();

			if (render_octree)
				octree.Draw();

			if (prefab_scene) {
				static float light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				static float light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
				glEnable(GL_LIGHT0);
			}

			// TODO : DELETE ------------------------

			//App->physx->DrawWorld();

			// -------------------------------------
		}

		if (base_game_object->HasChildren()) {
			if (isGameCamera) {
				OnPreCull(viewport->GetCamera());
			}

			std::vector<std::pair<float, GameObject*>> to_draw;
			std::vector<std::pair<float, GameObject*>> dynamic_to_draw;
			std::vector<std::pair<float, GameObject*>> static_to_draw;
			std::vector<std::pair<float, GameObject*>> to_draw_ui;
			std::vector<std::pair<float, GameObject*>> ui_2d;
			std::vector<std::pair<float, GameObject*>> ui_world;

			ComponentCamera* frustum_camera = viewport->GetCamera();

			if (check_culling_in_scene && App->renderer3D->actual_game_camera)
			{
				frustum_camera = App->renderer3D->actual_game_camera;
			}

			octree.SetStaticDrawList(&to_draw, frustum_camera);

			std::vector<GameObject*>::iterator item = base_game_object->children.begin();
			for (; item != base_game_object->children.end(); ++item) {
				if (*item != nullptr && (*item)->IsEnabled()) {
					(*item)->SetDrawList(&dynamic_to_draw, &to_draw_ui, frustum_camera);
				}
			}

			to_draw.insert(to_draw.end(),dynamic_to_draw.begin(), dynamic_to_draw.end());

			std::sort(dynamic_to_draw.begin(), dynamic_to_draw.end(), ModuleObjects::SortGameObjectToDraw);
			std::sort(to_draw.begin(), to_draw.end(), ModuleObjects::SortGameObjectToDraw);
			if (isGameCamera) {
				OnPreRender(viewport->GetCamera());
			}
			//predraw	
			CalculateShadows(dynamic_to_draw, viewport, static_to_draw, frustum_camera);

			glViewport(0, 0, viewport->GetSize().x, viewport->GetSize().y);
			glBindFramebuffer(GL_FRAMEBUFFER, viewport->GetFBO());

			//draw
			std::vector<std::pair<float, GameObject*>>::iterator it = to_draw.begin();
						
			viewport->GetCamera()->DrawSkybox(); 

			for (; it != to_draw.end(); ++it) {
				if ((*it).second != nullptr) {
					if (printing_scene)
						(*it).second->DrawScene(viewport->GetCamera());
					else
						(*it).second->DrawGame(viewport->GetCamera());
				}
			}
			
			UIOrdering(&to_draw_ui, &ui_2d, &ui_world);

			
			ComponentCamera* mainCamera = App->renderer3D->GetCurrentMainCamera();

			std::vector<std::pair<float, GameObject*>>::iterator it_ui_2d = ui_2d.begin();
			for (; it_ui_2d != ui_2d.end(); ++it_ui_2d) {
				if ((*it_ui_2d).second != nullptr) {
					ComponentUI* ui = (*it_ui_2d).second->GetComponent<ComponentUI>();
					if (ui != nullptr && ui->IsEnabled())
					{
						ui->Orientate(mainCamera);
						ui->Rotate();
						ui->Draw(!printing_scene);

					}
				}
			}

			if (printing_scene)
				OnDrawGizmos();
			if (isGameCamera) {
				OnPostRender(viewport->GetCamera());
			}
		}

		viewport->EndViewport();
	}

#else
	static bool light_view;
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	light_view = !light_view;

	if (!game_viewport->active || !game_viewport->CanRender() || game_viewport->GetCamera() == nullptr)
		return UPDATE_CONTINUE;

	game_viewport->BeginViewport();

	if (base_game_object->HasChildren()) {
		OnPreCull(game_viewport->GetCamera());

		std::vector<std::pair<float, GameObject*>> to_draw;
		std::vector<std::pair<float, GameObject*>> to_draw_ui;
		std::vector<std::pair<float, GameObject*>> ui_2d;
		std::vector<std::pair<float, GameObject*>> ui_world;
		std::vector<std::pair<float, GameObject*>> dynamic_to_draw;
		std::vector<std::pair<float, GameObject*>> static_to_draw;

		ComponentCamera* frustum_camera = game_viewport->GetCamera();

		octree.SetStaticDrawList(&to_draw, frustum_camera);

		octree.ShowAllStaticObjects(&static_to_draw, frustum_camera);

		std::vector<GameObject*>::iterator item = base_game_object->children.begin();
		for (; item != base_game_object->children.end(); ++item) {
			if (*item != nullptr && (*item)->IsEnabled()) {
				(*item)->SetDrawList(&dynamic_to_draw, &to_draw_ui, frustum_camera);
			}
		}

		to_draw.insert(to_draw.end(), dynamic_to_draw.begin(), dynamic_to_draw.end());

		std::sort(to_draw.begin(), to_draw.end(), ModuleObjects::SortGameObjectToDraw);
		std::sort(dynamic_to_draw.begin(), dynamic_to_draw.end(), ModuleObjects::SortGameObjectToDraw);
		std::sort(static_to_draw.begin(), static_to_draw.end(), ModuleObjects::SortGameObjectToDraw);

		OnPreRender(game_viewport->GetCamera());

		//predraw
		CalculateShadows(dynamic_to_draw, game_viewport, static_to_draw, frustum_camera);

		glViewport(0, 0, game_viewport->GetSize().x, game_viewport->GetSize().y);
		glBindFramebuffer(GL_FRAMEBUFFER, game_viewport->GetFBO());

		//draw game
		std::vector<std::pair<float, GameObject*>>::iterator it = to_draw.begin();

		game_viewport->GetCamera()->DrawSkybox();

		for (; it != to_draw.end(); ++it) {
			if ((*it).second != nullptr) {
				(*it).second->DrawGame(game_viewport->GetCamera());
			}
		}

		OnPostRender(game_viewport->GetCamera());

		UIOrdering(&to_draw_ui, &ui_2d, &ui_world);

		ComponentCamera* mainCamera = App->renderer3D->GetCurrentMainCamera();

		std::vector<std::pair<float, GameObject*>>::iterator it_ui_2d = ui_2d.begin();
		for (; it_ui_2d != ui_2d.end(); ++it_ui_2d) {
			if ((*it_ui_2d).second != nullptr) {
				ComponentUI* ui = (*it_ui_2d).second->GetComponent<ComponentUI>();
				if (ui != nullptr && ui->IsEnabled())
				{
					ui->Draw(!printing_scene);

				}
			}
		}
		std::vector<std::pair<float, GameObject*>>::iterator it_ui_world = ui_world.begin();
		for (; it_ui_world != ui_world.end(); ++it_ui_world) {
			if ((*it_ui_world).second != nullptr) {
				ComponentUI* ui = (*it_ui_world).second->GetComponent<ComponentUI>();
				if (ui != nullptr && ui->IsEnabled())
				{
					ui->Orientate(mainCamera);
					ui->Rotate();
					ui->Draw(!printing_scene);

				}
			}
		}
	}

	game_viewport->EndViewport();

	GLuint readFboId = 0;
	glGenFramebuffers(1, &readFboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, game_viewport->GetTexture(), 0);
	glBlitFramebuffer(0, 0, App->window->width, App->window->height,
		0, 0, App->window->width, App->window->height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &readFboId);

#endif
	return UPDATE_CONTINUE;
}

void ModuleObjects::CalculateShadows(std::vector<std::pair<float, GameObject*>>& dynamic_to_draw, Viewport* viewport, std::vector<std::pair<float, GameObject*>>& static_to_draw, ComponentCamera* frustum_camera)
{
	OPTICK_EVENT();
	for (std::list<DirLightProperties*>::const_iterator iter = directional_light_properites.begin(); iter != directional_light_properites.end(); iter++)
	{
		if (!(*iter)->light->castShadows)
			continue;

		glViewport(0, 0, 2048, 2048);
		glBindFramebuffer(GL_FRAMEBUFFER, (*iter)->depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		std::vector<std::pair<float, GameObject*>>::iterator it = dynamic_to_draw.begin();
		for (; it != dynamic_to_draw.end(); ++it) {
			if ((*it).second != nullptr && (*it).second->cast_shadow) {
				if (!printing_scene)
				{
					(*iter)->light->sizefrustrum = viewport->GetCamera()->frustum.farPlaneDistance * 0.5f;
					float3 camera_pos = viewport->GetCamera()->frustum.CenterPoint() / (*iter)->light->sizefrustrum;
					float3 camera_direction = viewport->GetCamera()->frustum.front;
					float halfFarPlaneD = (*iter)->light->sizefrustrum * 0.5f;
					float3 light_pos = float3((camera_pos.x - (*iter)->direction.x * halfFarPlaneD), (camera_pos.y - (*iter)->direction.y * halfFarPlaneD), (camera_pos.z - (*iter)->direction.z * halfFarPlaneD));

					glm::mat4 viewMatrix = glm::lookAt(glm::vec3((float)camera_pos.x, (float)camera_pos.y, (float)camera_pos.z),
						glm::vec3((float)light_pos.x, (float)light_pos.y, (float)-light_pos.z), 
						glm::vec3(0.0, 1.0, 0.0));

					(*iter)->viewMat.Set(&viewMatrix[0][0]);

					(*iter)->fake_position = light_pos;

					(*it).second->PreDrawGame(viewport->GetCamera(), (*iter)->viewMat, (*iter)->projMat, (*iter)->fake_position);
				}
				/*else
				(*it).second->PreDrawScene(viewport->GetCamera(), (*iter)->viewMat, (*iter)->projMat, (*iter)->fake_position);*/
			}
		}

		if ((*iter)->light->bakeShadows)
		{

			octree.ShowAllStaticObjects(&static_to_draw, frustum_camera);
			std::sort(static_to_draw.begin(), static_to_draw.end(), ModuleObjects::SortGameObjectToDraw);
			(*iter)->light->CalculateBakedViewMatrix();
			for (uint i = 0; i < 3; ++i)
			{
				glViewport(0, 0, 2048, 2048);
				(*iter)->light->BindForWriting(i);
				glClear(GL_DEPTH_BUFFER_BIT);
				std::vector<std::pair<float, GameObject*>>::iterator it2 = static_to_draw.begin();
				for (; it2 != static_to_draw.end(); ++it2) {
					if ((*it2).second != nullptr && (*it2).second->cast_shadow) {
						(*it2).second->PreDrawGame(game_viewport->GetCamera(), (*iter)->light->viewMatrix[i], (*iter)->light->projMatrix, (*iter)->fake_position_baked[i]);
					}
				}
				(*iter)->light->bakeShadows = false;
			}
		}
	}
}

void ModuleObjects::DrawRay()
{
	if (App->camera->ray.IsFinite()) {
		glColor3f(ray_color.r, ray_color.g, ray_color.b);
		glLineWidth(ray_width);
		glBegin(GL_LINES);

		glVertex3f(App->camera->ray.a.x, App->camera->ray.a.y, App->camera->ray.a.z);
		glVertex3f(App->camera->ray.b.x, App->camera->ray.b.y, App->camera->ray.b.z);

		glEnd();
		glLineWidth(1);
	}
}

bool ModuleObjects::CleanUp()
{
	OPTICK_EVENT();
	Gizmos::ClearAllCurrentGizmos();

	if (Time::IsInGameState()) {
		CleanUpScriptsOnStop();
	}

	delete base_game_object;
	base_game_object = nullptr;

	if (octree.root != nullptr) {
		delete octree.root;
		octree.root = nullptr;
	}

	DeleteReturns();

	for (Viewport* viewport : viewports) {
		delete viewport;
	}

	return true;
}

void ModuleObjects::ChangeWireframeMode()
{
	wireframe_mode = !wireframe_mode;
	base_game_object->ChangeWireframe(wireframe_mode);
}

void ModuleObjects::ChangeViewMeshMode()
{
	view_mesh_mode = !view_mesh_mode;
	base_game_object->ChangeMeshView(view_mesh_mode);
}

void ModuleObjects::DeleteAllObjects()
{
	std::vector<GameObject*>::iterator item = base_game_object->children.begin();
	while (item != base_game_object->children.end()) {
		if (*item != nullptr) {
			delete* item;
			*item = nullptr;
			item = base_game_object->children.erase(item);
		}
		else
			++item;
	}
	game_objects_selected.clear();
}

void ModuleObjects::ChangeEnableGrid()
{
	allow_grid = !allow_grid;
}

void ModuleObjects::ChangeEnableOutline()
{
	outline = !outline;
}


void ModuleObjects::ChangeEnableNormalVertex()
{
	draw_vertex_normals = !draw_vertex_normals;
	base_game_object->ChangeVertexNormalsView(draw_vertex_normals);
}

void ModuleObjects::ChangeEnableNormalFace()
{
	draw_face_normals = !draw_face_normals;
	base_game_object->ChangeFaceNormalsView(draw_face_normals);
}

void ModuleObjects::ChangeEnableAABB()
{
	draw_all_AABB = !draw_all_AABB;
	base_game_object->ChangeAABB(draw_all_AABB);
}

void ModuleObjects::ChangeEnableOctree()
{
	render_octree = !render_octree;
}

void ModuleObjects::ChangeEnableOBB()
{
	draw_all_OBB = !draw_all_OBB;
	base_game_object->ChangeOBB(draw_all_OBB);
}

void ModuleObjects::SetNewSelectedObject(GameObject* object_selected, bool select_children)
{
	bool exists = std::find(game_objects_selected.begin(), game_objects_selected.end(), object_selected) != game_objects_selected.end();

	if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {
		object_selected->ChangeSelected(!exists);
		if (!exists) {
			game_objects_selected.push_back(object_selected);
			if (select_children) {
				object_selected->open_node = true;
				std::stack<GameObject*> objects;
				for (auto item = object_selected->children.begin(); item != object_selected->children.end(); ++item) {
					objects.push(*item);
				}
				while (!objects.empty()) {
					GameObject* obj = objects.top();
					objects.pop();

					obj->ChangeSelected(true);
					obj->open_node = true;
					game_objects_selected.push_back(obj);
					for (auto item = obj->children.begin(); item != obj->children.end(); ++item) {
						objects.push(*item);
					}

				}
			}
		}
		else {
			game_objects_selected.remove(object_selected);
		}
	}
	else {
		auto item = game_objects_selected.begin();
		for (; item != game_objects_selected.end(); ++item) {
			if (*item != object_selected) {
				(*item)->ChangeSelected(false);
			}
		}
		game_objects_selected.clear();
		game_objects_selected.push_back(object_selected);
		if (!exists) {
			object_selected->ChangeSelected(true);
		}
		if (select_children) {
			object_selected->open_node = true;
			std::stack<GameObject*> objects;
			for (auto item = object_selected->children.begin(); item != object_selected->children.end(); ++item) {
				objects.push(*item);
			}
			while (!objects.empty()) {
				GameObject* obj = objects.top();
				objects.pop();

				obj->ChangeSelected(true);
				obj->open_node = true;
				game_objects_selected.push_back(obj);
				for (auto item = obj->children.begin(); item != obj->children.end(); ++item) {
					objects.push(*item);
				}

			}
		}
	}
	App->renderer3D->selected_game_camera = (ComponentCamera*)object_selected->GetComponent(ComponentType::CAMERA);

#ifndef GAME_VERSION
	if (App->ui->panel_project->selected_resource != nullptr)
	{
		App->SendAlienEvent(App->ui->panel_project->selected_resource, AlienEventType::RESOURCE_DESELECTED);
		App->ui->panel_project->selected_resource = nullptr;
	}
#endif // !GAME_VERSION

	App->CastEvent(EventType::ON_GO_SELECT);

}

const std::list<GameObject*>& ModuleObjects::GetSelectedObjects()
{
	return game_objects_selected;
}

void ModuleObjects::DeselectObjects()
{
	auto item = game_objects_selected.begin();
	for (; item != game_objects_selected.end(); ++item) {
		(*item)->ChangeSelected(false);
	}
	game_objects_selected.clear();
	App->renderer3D->selected_game_camera = nullptr;
}

void ModuleObjects::DeselectObject(GameObject* obj)
{
	game_objects_selected.remove(obj);
	obj->ChangeSelected(false);
}

void ModuleObjects::OnPlay() const
{
	InitScripts();
}

void ModuleObjects::InitScripts() const
{
	// scripts awake
	auto to_iter = current_scripts;
	std::list<Alien*>::const_iterator item = to_iter.cbegin();
	for (; item != to_iter.cend(); ++item) {
		if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
			try {
				(*item)->Awake();
			}
			catch (...)
			{
				try {
					LOG_ENGINE("CODE ERROR IN THE AWAKE OF THE SCRIPT: %s", (*item)->data_name);
				}
				catch (...) {
					LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS AWAKE");
				}
#ifndef GAME_VERSION
				App->ui->SetError();
#endif
			}
		}
	}
	// scripts start
	item = current_scripts.cbegin();
	for (; item != current_scripts.cend(); ++item) {
		if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
			try {
				(*item)->Start();
			}
			catch (...)
			{
				try {
					LOG_ENGINE("CODE ERROR IN THE START OF THE SCRIPT: %s", (*item)->data_name);
				}
				catch (...) {
					LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS START");
				}
#ifndef GAME_VERSION
				App->ui->SetError();
#endif
			}
		}
	}
}

void ModuleObjects::ScriptsPreUpdate() const
{
	if ((Time::state == Time::GameState::PLAY || Time::state == Time::GameState::PLAY_ONCE) && !current_scripts.empty()) {
		auto to_iter = current_scripts;
		std::list<Alien*>::const_iterator item = to_iter.cbegin();
		for (; item != to_iter.cend(); ++item) {
			if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
				try {
					(*item)->PreUpdate();
				}
				catch (...)
				{
					try {
						LOG_ENGINE("CODE ERROR IN THE PREUPDATE OF THE SCRIPT: %s", (*item)->data_name);
					}
					catch (...) {
						LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS PREUPDATE");
					}
#ifndef GAME_VERSION
					App->ui->SetError();
#endif
				}
			}
		}
	}
}

void ModuleObjects::ScriptsUpdate() const
{
	if ((Time::state == Time::GameState::PLAY || Time::state == Time::GameState::PLAY_ONCE) && !current_scripts.empty()) {
		auto to_iter = current_scripts;
		std::list<Alien*>::const_iterator item = to_iter.cbegin();
		for (; item != to_iter.cend(); ++item) {
			if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
				try {
					(*item)->Update();
				}
				catch (...)
				{
					try {
						LOG_ENGINE("CODE ERROR IN THE UPDATE OF THE SCRIPT: %s", (*item)->data_name);
					}
					catch (...) {
						LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS UPDATE");
					}
#ifndef GAME_VERSION
					App->ui->SetError();
#endif
				}
			}
		}
	}
}

void ModuleObjects::ScriptsPostUpdate() const
{
	if ((Time::state == Time::GameState::PLAY || Time::state == Time::GameState::PLAY_ONCE) && !current_scripts.empty()) {
		auto to_iter = current_scripts;
		std::list<Alien*>::const_iterator item = to_iter.cbegin();
		for (; item != to_iter.cend(); ++item) {
			if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
				try {
					(*item)->PostUpdate();
				}
				catch (...)
				{
					try {
						LOG_ENGINE("CODE ERROR IN THE POSTUPDATE OF THE SCRIPT: %s", (*item)->data_name);
					}
					catch (...) {
						LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS POSTUPDATE");
					}
#ifndef GAME_VERSION
					App->ui->SetError();
#endif
				}
			}
		}
	}
}

void ModuleObjects::CleanUpScriptsOnStop() const
{
	auto to_iter = current_scripts;
	std::list<Alien*>::const_iterator item = to_iter.cbegin();
	for (; item != to_iter.cend(); ++item) {
		if (*item != nullptr && (*item)->game_object != nullptr) {
			try {
				(*item)->CleanUp();
			}
			catch (...)
			{
				try {
					LOG_ENGINE("CODE ERROR IN THE CLEANUP OF THE SCRIPT: %s", (*item)->data_name);
				}
				catch (...) {
					LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS CLEANUP");
				}
#ifndef GAME_VERSION
				App->ui->SetError();
#endif
			}
		}
	}
}

void ModuleObjects::OnDrawGizmos() const
{
	Gizmos::controller = !Gizmos::controller;
	// scripts OnDrawGizmos
	for (std::list<Alien*>::const_iterator item = current_scripts.cbegin(); item != current_scripts.cend(); ++item) {
		if (*item != nullptr && (*item)->game_object != nullptr && (*item)->game_object->parent_enabled && (*item)->game_object->enabled && (*item)->IsScriptEnabled()) {
			try {
				(*item)->OnDrawGizmos();
			}
			catch (...)
			{
				try {
					LOG_ENGINE("CODE ERROR IN THE ONDRAWGIZMOS OF THE SCRIPT: %s", (*item)->data_name);
				}
				catch (...) {
					LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS ONDRAWGIZMOS");
				}
#ifndef GAME_VERSION
				App->ui->SetError();
#endif
			}
		}
	}
	// scripts OnDrawGizmosSelected
	auto item = game_objects_selected.cbegin();
	for (; item != game_objects_selected.cend(); ++item) {
		if (*item != nullptr && (*item)->parent_enabled && (*item)->enabled) {
			std::vector<ComponentScript*> scripts = (*item)->GetComponents<ComponentScript>();
			for (uint i = 0; i < scripts.size(); ++i) {
				if (scripts[i] != nullptr && scripts[i]->IsEnabled() && scripts[i]->need_alien) {
					Alien* alien = (Alien*)scripts[i]->data_ptr;
					if (alien != nullptr) {
						try {
							alien->OnDrawGizmosSelected();
						}
						catch (...)
						{
							try {
								LOG_ENGINE("CODE ERROR IN THE ONDRAWGIZMOSSELECTED OF THE SCRIPT: %s", alien->data_name);
							}
							catch (...) {
								LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS ONDRAWGIZMOSSELECTED");
							}
#ifndef GAME_VERSION
							App->ui->SetError();
#endif
						}
					}
				}
			}
		}
	}
	Gizmos::RemoveGizmos();
}

void ModuleObjects::OnPreCull(ComponentCamera* camera) const
{
	if (camera != nullptr && camera->game_object_attached != nullptr) {
		std::vector<ComponentScript*> script_cameras = camera->game_object_attached->GetComponents<ComponentScript>();
		auto item = script_cameras.begin();
		for (; item != script_cameras.end(); ++item) {
			if (*item != nullptr && (*item)->need_alien && (*item)->data_ptr != nullptr) {
				Alien* alien = (Alien*)(*item)->data_ptr;
				if (alien != nullptr) {
					try {
						alien->OnPreCull();
					}
					catch (...)
					{
						try {
							LOG_ENGINE("CODE ERROR IN THE ONPRECULL OF THE SCRIPT: %s", alien->data_name);
						}
						catch (...) {
							LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS ONPRECULL");
						}
#ifndef GAME_VERSION
						App->ui->SetError();
#endif
					}
				}
			}
		}
	}
}

void ModuleObjects::OnPreRender(ComponentCamera* camera) const
{
	if (camera != nullptr && camera->game_object_attached != nullptr) {
		std::vector<ComponentScript*> script_cameras = camera->game_object_attached->GetComponents<ComponentScript>();
		auto item = script_cameras.begin();
		for (; item != script_cameras.end(); ++item) {
			if (*item != nullptr && (*item)->need_alien && (*item)->data_ptr != nullptr) {
				Alien* alien = (Alien*)(*item)->data_ptr;
				if (alien != nullptr) {
					try {
						alien->OnPreRender();
					}
					catch (...)
					{
						try {
							LOG_ENGINE("CODE ERROR IN THE ONPRERENDER OF THE SCRIPT: %s", alien->data_name);
						}
						catch (...) {
							LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS ONPRERENDER");
						}
#ifndef GAME_VERSION
						App->ui->SetError();
#endif
					}
				}
			}
		}
	}
}

void ModuleObjects::OnPostRender(ComponentCamera* camera) const
{
	if (camera != nullptr && camera->game_object_attached != nullptr) {
		std::vector<ComponentScript*> script_cameras = camera->game_object_attached->GetComponents<ComponentScript>();
		auto item = script_cameras.begin();
		for (; item != script_cameras.end(); ++item) {
			if (*item != nullptr && (*item)->need_alien && (*item)->data_ptr != nullptr) {
				Alien* alien = (Alien*)(*item)->data_ptr;
				if (alien != nullptr) {
					try {
						alien->OnPostRender();
					}
					catch (...)
					{
						try {
							LOG_ENGINE("CODE ERROR IN THE ONPOSTRENDER OF THE SCRIPT: %s", alien->data_name);
						}
						catch (...) {
							LOG_ENGINE("UNKNOWN ERROR IN SCRIPTS ONPOSTRENDER");
						}
#ifndef GAME_VERSION
						App->ui->SetError();
#endif
					}
				}
			}
		}
	}
}

GameObject* ModuleObjects::CreateEmptyGameObject(GameObject* parent, bool set_selected)
{
	GameObject* object = nullptr;

	if (parent != nullptr) {
		parent->open_node = true;
		object = new GameObject(parent);
		object->SetName("Empty Child");
	}
	else {
		object = new GameObject(GetRoot(false));
		object->SetName("Empty GameObject");
	}

	if (set_selected)
		SetNewSelectedObject(object, false);

	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, object);

	return object;
}

void ModuleObjects::MoveObjectDown(GameObject* object, bool bottom)
{
	GameObject* parent = object->parent;

	if (object == parent->children.back())
		return;

	if (bottom) {
		parent->children.erase(std::find(parent->children.begin(), parent->children.end(), object));
		parent->AddChild(object);
	}
	else {
		std::vector<GameObject*>::iterator item = parent->children.begin();
		for (; item != parent->children.end(); ++item) {
			if (*item != nullptr) {
				if (*item == object) {
					item = parent->children.erase(item);
					parent->children.insert(++item, object);
					break;
				}
			}
		}
	}
}

void ModuleObjects::MoveObjectUp(GameObject* object, bool top)
{
	GameObject* parent = object->parent;

	if (object == parent->children.front())
		return;

	if (top) {
		parent->children.erase(std::find(parent->children.begin(), parent->children.end(), object));
		parent->children.insert(parent->children.begin(), object);
	}
	else {
		std::vector<GameObject*>::iterator item = parent->children.begin();
		for (; item != parent->children.end(); ++item) {
			if (*item != nullptr) {
				if (*item == object) {
					item = parent->children.erase(item);
					parent->children.insert(--item, object);
					break;
				}
			}
		}
	}
}

void ModuleObjects::MoveComponentDown(GameObject* object, Component* component, bool bottom)
{
	if (component == object->components.back())
		return;

	if (bottom) {
		object->components.erase(std::find(object->components.begin(), object->components.end(), component));
		object->components.push_back(component);
	}
	else {
		std::vector<Component*>::iterator item = object->components.begin();
		for (; item != object->components.end(); ++item) {
			if (*item != nullptr) {
				if (*item == component) {
					item = object->components.erase(item);
					object->components.insert(++item, component);
					break;
				}
			}
		}
	}
}

void ModuleObjects::MoveComponentUp(GameObject* object, Component* component, bool top)
{
	if (component == object->components.at(1))
		return;

	if (top) {
		object->components.erase(std::find(object->components.begin(), object->components.end(), component));
		object->components.insert(object->components.begin() + 1, component);
	}
	else {
		std::vector<Component*>::iterator item = object->components.begin();
		for (; item != object->components.end(); ++item) {
			if (*item != nullptr) {
				if (*item == component) {
					item = object->components.erase(item);
					object->components.insert(--item, component);
					break;
				}
			}
		}
	}
}

GameObject* ModuleObjects::GetGameObjectByID(const u64& id)
{
	return base_game_object->GetGameObjectByID(id);
}

void ModuleObjects::ReparentGameObject(GameObject* object, GameObject* next_parent, bool to_cntrlZ)
{
	if (object != nullptr && next_parent != nullptr && !object->Exists(next_parent)) {
		to_reparent.push_back({ object,next_parent,to_cntrlZ });
	}
}

void ModuleObjects::SaveScene(ResourceScene* to_load_scene, const char* force_with_path)
{
	OPTICK_EVENT();
	App->CastEvent(EventType::ON_SAVE);

	if (to_load_scene == nullptr && force_with_path == nullptr) {
		LOG_ENGINE("Scene to load was nullptr");
		return;
	}

	std::string meta_path;
	JSON_Value* value = json_value_init_object();
	JSON_Object* object = json_value_get_object(value);

	if (force_with_path == nullptr) {
		// remove the last save and save the new
		remove(to_load_scene->GetLibraryPath());
		meta_path = to_load_scene->GetAssetsPath();
		remove(meta_path.data());
	}
	else {
		meta_path = std::string(force_with_path);
	}

	json_serialize_to_file_pretty(value, meta_path.data());

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* scene = new JSONfilepack(meta_path.data(), object, value);

		scene->StartSave();

		if (force_with_path == nullptr) {
			scene->SetString("Scene.Name", to_load_scene->GetName());
		}
		else {
			scene->SetString("Scene.Name", "NONE");
		}

		// get scene root
		GameObject* scene_root = nullptr;
		if (to_load_scene == nullptr) {
			scene_root = base_game_object;
		}
		else {
			for (auto item = base_game_object->children.begin(); item != base_game_object->children.end(); ++item) {
				if ((*item)->ID == to_load_scene->GetID()) {
					scene_root = *item;
					break;
				}
			}
		}

		if (current_scenes.empty()) {
			if (to_load_scene != nullptr) {
				scene_root->SetName(to_load_scene->GetName());
				scene_root->ID = to_load_scene->GetID();
				current_scenes.push_back(to_load_scene);
			}
			else {
				scene_root->SetName("Untitled*");
			}
		}

		if (!scene_root->children.empty()) { // if scene_root has children, save them
			JSONArraypack* game_objects = scene->InitNewArray("Scene.GameObjects");

			game_objects->SetAnotherNode();

			std::vector<GameObject*>::iterator item = scene_root->children.begin();
			for (; item != scene_root->children.end(); ++item) {
				if (*item != nullptr) {
					SaveGameObject(*item, game_objects, 1);
					if ((*item) != scene_root->children.back())
						game_objects->SetAnotherNode();
				}
			}
		}

		scene->FinishSave();
		delete scene;
		if (force_with_path == nullptr) {
			App->file_system->Copy(to_load_scene->GetAssetsPath(), to_load_scene->GetLibraryPath());
		}
	}
	else {
		LOG_ENGINE("Could not load scene, fail when creating the file");
	}
}

void ModuleObjects::LoadScene(const char* name, bool change_scene)
{
	OPTICK_EVENT();
	App->audio->Stop();
	ResourceScene* to_load = App->resources->GetSceneByName(name);
	if (to_load != nullptr || !change_scene) {

		std::string path;
		if (to_load != nullptr) {
			path = to_load->GetLibraryPath();
		}
		else {
			path = name;
		}

		JSON_Value* value = json_parse_file(path.data());
		JSON_Object* object = json_value_get_object(value);

		if (value != nullptr && object != nullptr)
		{
			App->CastEvent(EventType::ON_UNLOAD_SCENE);
			octree.Clear();
			Gizmos::ClearAllCurrentGizmos();
			delete base_game_object;
			game_objects_selected.clear();
			base_game_object = new GameObject();
			base_game_object->ID = 0;
			base_game_object->is_static = true;

			GameObject* scene_root = nullptr;
			if (!change_scene) {
				scene_root = base_game_object;
			}
			else {
				scene_root = new GameObject();
				scene_root->ID = to_load->GetID();
				scene_root->is_static = true;
				scene_root->SetName(name);
				scene_root->parent = base_game_object;
				base_game_object->children.push_back(scene_root);
			}

			if (to_load != nullptr) {
				scene_active = to_load->GetID();
			}

			if (Time::IsInGameState()) {
				CleanUpScriptsOnStop();
			}
			current_scripts.clear();

			JSONfilepack* scene = new JSONfilepack(path.data(), object, value);

			JSONArraypack* game_objects = scene->GetArray("Scene.GameObjects");

			if (game_objects != nullptr) {
				std::vector<GameObject*> objects_created;

				for (uint i = 0; i < game_objects->GetArraySize(); ++i) {
					GameObject* obj = new GameObject(true);
					u64 parentID = std::stoull(game_objects->GetString("ParentID"));
					if (parentID != scene_root->ID) {
						std::vector<GameObject*>::iterator objects = objects_created.begin();
						for (; objects != objects_created.end(); ++objects) {
							if ((*objects)->ID == parentID) {
								obj->LoadObject(game_objects, *objects);
								break;
							}
						}
					}
					else {
						obj->LoadObject(game_objects, scene_root);
					}
					objects_created.push_back(obj);
					game_objects->GetAnotherNode();
				}
				for each (GameObject * obj in objects_created) //not sure where to place this, need to link skeletons to meshes after all go's have been created
				{
					ComponentDeformableMesh* def_mesh = obj->GetComponent<ComponentDeformableMesh>();
					if (def_mesh)
						def_mesh->AttachSkeleton();
				}
				ReAttachUIScriptEvents();
				delete scene;

				if (change_scene) {
					struct stat file;
					stat(path.data(), &file);

					// refresh prefabs if are not locked
					std::vector<GameObject*> prefab_roots;
					base_game_object->GetAllPrefabRoots(prefab_roots);

					for (uint i = 0; i < prefab_roots.size(); ++i) {
						if (prefab_roots[i] != nullptr && !prefab_roots[i]->prefab_locked) {
							ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(prefab_roots[i]->GetPrefabID());
							if (prefab != nullptr && prefab->GetID() != 0) {
								struct stat prefab_file;
								// TODO: when passing to library change
								if (stat(prefab->GetAssetsPath(), &prefab_file) == 0) {
									if (prefab_file.st_mtime > file.st_mtime) {
										auto find = prefab_roots[i]->parent->children.begin();
										for (; find != prefab_roots[i]->parent->children.end(); ++find) {
											if (*find == prefab_roots[i]) {
												prefab->ConvertToGameObjects(prefab_roots[i]->parent, find - prefab_roots[i]->parent->children.begin(), (*find)->GetComponent<ComponentTransform>()->GetGlobalPosition());
												prefab_roots[i]->ToDelete();
												break;
											}
										}
									}
								}
							}
						}
					}
					DeleteReturns();
				}

				if (!to_add.empty()) {
					auto item = to_add.begin();
					for (; item != to_add.end(); ++item) {
						GameObject* found = GetGameObjectByID((*item).first);
						if (found != nullptr) {
							*(*item).second = found;
						}
					}
				}

				if (!current_scripts.empty() && Time::IsInGameState()) {
					OnPlay();
					for each (GameObject * obj in objects_created) //not sure where to place this, need to link skeletons to meshes after all go's have been created
					{
						ComponentAnimator* anim = obj->GetComponent<ComponentAnimator>();
						if (anim != nullptr) {
							anim->OnPlay();
						}
					}
				}
			}
			if (change_scene) {
				current_scenes.clear();
				current_scenes.push_back(to_load);
			}
		}
		else {
			LOG_ENGINE("Error loading scene %s", path.data());
		}
	}
}

void ModuleObjects::OpenCoScene(const char* name)
{
	OPTICK_EVENT();
	ResourceScene* to_load = App->resources->GetSceneByName(name);
	if (to_load != nullptr) {

		for (auto item = App->objects->GetGlobalRoot()->children.begin(); item != App->objects->GetGlobalRoot()->children.end(); ++item) {
			if ((*item)->ID == to_load->GetID()) {
				return;
			}
		}

		JSON_Value* value = json_parse_file(to_load->GetLibraryPath());
		JSON_Object* object = json_value_get_object(value);

		if (value != nullptr && object != nullptr) {
			current_scenes.push_back(to_load);
			GameObject* scene_root = new GameObject();
			scene_root->ID = to_load->GetID();
			scene_root->is_static = true;
			scene_root->SetName(name);
			scene_root->parent = base_game_object;
			base_game_object->children.push_back(scene_root);


			JSONfilepack* scene = new JSONfilepack(to_load->GetLibraryPath(), object, value);

			JSONArraypack* game_objects = scene->GetArray("Scene.GameObjects");

			if (game_objects != nullptr) {
				std::vector<GameObject*> objects_created;

				for (uint i = 0; i < game_objects->GetArraySize(); ++i) {
					GameObject* obj = new GameObject(true);
					u64 parentID = std::stoull(game_objects->GetString("ParentID"));
					if (parentID != scene_root->ID) {
						std::vector<GameObject*>::iterator objects = objects_created.begin();
						for (; objects != objects_created.end(); ++objects) {
							if ((*objects)->ID == parentID) {
								obj->LoadObject(game_objects, *objects);
								break;
							}
						}
					}
					else {
						obj->LoadObject(game_objects, scene_root);
					}
					objects_created.push_back(obj);
					game_objects->GetAnotherNode();
				}
				for each (GameObject * obj in objects_created) //not sure where to place this, need to link skeletons to meshes after all go's have been created
				{
					ComponentDeformableMesh* def_mesh = obj->GetComponent<ComponentDeformableMesh>();
					if (def_mesh)
						def_mesh->AttachSkeleton();
				}
				ReAttachUIScriptEvents();
				delete scene;
			}
		}

	}
}

void ModuleObjects::CreateEmptyScene()
{
	current_scenes.clear();

	delete base_game_object;
	game_objects_selected.clear();
	base_game_object = new GameObject();
	base_game_object->ID = 0;
	base_game_object->is_static = true;

	GameObject* scene_root = new GameObject();
	scene_root->ID = App->resources->GetRandomID();
	scene_root->is_static = true;
	scene_root->SetName("Untitled*");
	scene_root->parent = base_game_object;
	scene_active = scene_root->ID;
	base_game_object->children.push_back(scene_root);

	GameObject* camera = new GameObject(scene_root);
	camera->SetName("Main Camera");
	camera->AddComponent(new ComponentCamera(camera));

	GameObject* light = new GameObject(scene_root);
	light->SetName("Directional Light");
	light->AddComponent(new ComponentLightDirectional(light));
	light->transform->SetGlobalRotation(math::Quat::LookAt(float3::unitZ(), float3(-0.5f, -0.5f, 0.5f), float3::unitY(), float3::unitY()));
}

void ModuleObjects::SaveGameObject(GameObject* obj, JSONArraypack* to_save, const uint& family_number)
{
	OPTICK_EVENT();
	obj->SaveObject(to_save, family_number);

	std::vector<GameObject*>::iterator item = obj->children.begin();
	for (; item != obj->children.end(); ++item) {
		if (*item != nullptr) {
			to_save->SetAnotherNode();
			SaveGameObject(*item, to_save, family_number + 1);
		}
	}
}

GameObject* ModuleObjects::GetRoot(bool ignore_prefab)
{
	if (prefab_scene && !ignore_prefab) {
		if (base_game_object->children.size() == 0)
			return base_game_object;
		else
			return base_game_object->children.back();
	}
	else {
		for (auto item = base_game_object->children.begin(); item != base_game_object->children.end(); ++item) {
			if ((*item)->ID == scene_active) {
				return *item;
			}
		}
		return nullptr;
	}
}

GameObject* ModuleObjects::GetGlobalRoot()
{
	return base_game_object;
}

void ModuleObjects::CreateRoot()
{
	delete base_game_object;
	game_objects_selected.clear();
	base_game_object = new GameObject();
	base_game_object->ID = 0;
	base_game_object->is_static = true;
}

void ModuleObjects::SwapReturnZ(bool get_save, bool delete_current)
{
	if (delete_current) {
		DeleteReturns();
	}

	if (get_save) {
		return_actions = save_return_actions;
		fordward_actions = save_fordward_actions;

		while (!save_return_actions.empty()) {
			save_return_actions.pop();
		}
		while (!save_fordward_actions.empty()) {
			save_fordward_actions.pop();
		}
	}
	else {
		save_return_actions = return_actions;
		save_fordward_actions = fordward_actions;

		while (!return_actions.empty()) {
			return_actions.pop();
		}
		while (!fordward_actions.empty()) {
			fordward_actions.pop();
		}
	}
}

void ModuleObjects::HotReload()
{
	inHotReload = true;
	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);
	json_serialize_to_file_pretty(value, "Library/ScriptsTEMP.alien");

	if (value != nullptr && json_object != nullptr) {
		JSONfilepack* to_save = new JSONfilepack("Library/ScriptsTEMP.alien", json_object, value);
		to_save->StartSave();
		if (current_scripts.empty()) {
			to_save->SetBoolean("AreScripts", false);
		}
		else {
			to_save->SetBoolean("AreScripts", true);
			JSONArraypack* scripts = to_save->InitNewArray("Arr.Scripts");
			CreateJsonScript(base_game_object, scripts);
		}
		to_save->FinishSave();
		delete to_save;
	}
	current_scripts.clear();
	if (FreeLibrary(App->scripts_dll)) {
		LOG_ENGINE("Dll correctly unloaded");
		if (remove(DLL_WORKING_PATH) == 0) {
			LOG_ENGINE("Dll correctly removed");
			App->resources->ReloadScripts();
			// if this change is done without waiting, we cant move the file because the creating dll process hasn't finished
			// so I just wait until MoveFileA succed. 
			// If someone knows how to know when an extern process finishes with a file, tell me please contrasnya@gmail.com
			while (MoveFileA(DLL_CREATION_PATH, DLL_WORKING_PATH) == FALSE) {}
			LOG_ENGINE("New Dll correctly moved");
			App->scripts_dll = nullptr;
			App->scripts_dll = LoadLibrary(App->dll.data());
			if (App->scripts_dll != nullptr) {
				JSON_Value* value_load = json_parse_file("Library/ScriptsTEMP.alien");
				JSON_Object* json_object_load = json_value_get_object(value_load);

				if (value_load != nullptr && json_object_load != nullptr) {
					JSONfilepack* to_load = new JSONfilepack("Library/ScriptsTEMP.alien", json_object_load, value_load);
					if (to_load->GetBoolean("AreScripts")) {
						JSONArraypack* scripts_to_load = to_load->GetArray("Arr.Scripts");
						ReAssignScripts(scripts_to_load);
						errors = false;
						if (Time::IsInGameState()) {
							auto item = current_scripts.begin();
							for (; item != current_scripts.end(); ++item) {
								if (*item != nullptr) {
									(*item)->Awake();
									(*item)->Start();
								}
							}
						}
					}
					remove("Library/ScriptsTEMP.alien");
					delete to_load;
				}
			}
		}
	}
	inHotReload = false;
}

bool ModuleObjects::SortGameObjectToDraw(std::pair<float, GameObject*> first, std::pair<float, GameObject*> last)
{
	//orders from near to far
	return first.first > last.first;
}

bool ModuleObjects::SortUIToDraw(std::pair<float, GameObject*> first, std::pair<float, GameObject*> last)
{
	//orders from far to near
	return first.first < last.first;
}

void ModuleObjects::AddScriptObject(const u64& ID, GameObject** object)
{
	to_add.push_back({ ID, object });
}

void ModuleObjects::DuplicateObjects()
{
	if (App->camera->is_scene_focused || App->ui->panel_hierarchy->is_focused) {
		std::vector<GameObject*> new_selected;
		auto item = game_objects_selected.begin();
		for (; item != game_objects_selected.end(); ++item) {
			if (*item != nullptr) {
				new_selected.push_back((*item)->Clone());
			}
		}
		DeselectObjects();
		auto item2 = new_selected.begin();
		for (; item2 != new_selected.end(); ++item2) {
			if (*item2 != nullptr) {
				(*item2)->ChangeSelected(true);
				game_objects_selected.push_back(*item2);
			}
		}
	}
}

void ModuleObjects::AddInvoke(std::function<void()> void_no_params_function, const float& second, Alien* alien)
{
	InvokeInfo* info = new InvokeInfo();
	info->function = void_no_params_function;
	info->time_to_wait = second;
	info->alien = alien;
	info->is_repeating = false;
	info->time_started = Time::time_since_start;
	info->ID = App->resources->GetRandomID();

	invokes.push_back(info);
}

void ModuleObjects::AddInvokeRepeating(std::function<void()> void_no_params_function, const float& second, const float& seconds_between_each_call, Alien* alien)
{
	InvokeInfo* info = new InvokeInfo();
	info->function = void_no_params_function;
	info->time_to_wait = second;
	info->alien = alien;
	info->is_repeating = true;
	info->time_between = seconds_between_each_call;
	info->time_started = Time::time_since_start;
	info->ID = App->resources->GetRandomID();

	invokes.push_back(info);
}

void ModuleObjects::CancelInvokes(Alien* alien)
{
	auto invokes_list = invokes;
	auto item = invokes_list.begin();
	for (; item != invokes_list.end(); ++item) {
		if ((*item)->alien == alien) {
			invokes.remove(*item);
			delete* item;
			*item = nullptr;
		}
	}
}

void ModuleObjects::ReAttachUIScriptEvents()
{
	std::stack<GameObject*> objects;
	objects.push(base_game_object);

	while (!objects.empty()) {
		GameObject* obj = objects.top();
		objects.pop();

		std::vector<ComponentScript*> scriptsVec = obj->GetComponents<ComponentScript>();
		if (!scriptsVec.empty()) {
			ComponentButton* button = obj->GetComponent<ComponentButton>();
			if (button != nullptr) {
				CompareName(&button->listenersOnClick, scriptsVec);
				CompareName(&button->listenersOnClickRepeat, scriptsVec);
				CompareName(&button->listenersOnHover, scriptsVec);
				CompareName(&button->listenersOnRelease, scriptsVec);
				CompareName(&button->listenersOnExit, scriptsVec);
				CompareName(&button->listenersOnEnter, scriptsVec);
			}
			else {
				ComponentCheckbox* checkbox = obj->GetComponent<ComponentCheckbox>();
				if (checkbox != nullptr) {
					CompareName(&checkbox->listenersOnClick, scriptsVec);
					CompareName(&checkbox->listenersOnClickRepeat, scriptsVec);
					CompareName(&checkbox->listenersOnHover, scriptsVec);
					CompareName(&checkbox->listenersOnRelease, scriptsVec);
					CompareName(&checkbox->listenersOnExit, scriptsVec);
					CompareName(&checkbox->listenersOnEnter, scriptsVec);

				}
			}
		}

		for (auto item = obj->children.begin(); item != obj->children.end(); ++item) {
			objects.push(*item);
		}
	}
}

void ModuleObjects::ResetUIFocus()
{
	if (GetGameObjectByID(selected_ui) != nullptr && selected_ui != -1) {
		GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Idle;
		selected_ui = -1;
	}

	first_assigned_selected = false;
}


//bool ModuleObjects::IsInvoking(std::function<void()> void_no_params_function)
//{
//	auto item = invokes.begin();
//	for (; item != invokes.end(); ++item) {
//		if ((*item). == CompareFunction<void>(void_no_params_function)) {
//			return true;
//		}
//	}
//	return false;
//}

void ModuleObjects::CreateJsonScript(GameObject* obj, JSONArraypack* to_save)
{
	if (obj->HasChildren()) {
		std::vector<GameObject*>::iterator item = obj->children.begin();
		for (; item != obj->children.end(); ++item) {
			if (*item != nullptr) {
				std::vector<ComponentScript*> scripts = (*item)->GetComponents<ComponentScript>();
				if (!scripts.empty()) {
					std::vector<ComponentScript*>::iterator script = scripts.begin();
					for (; script != scripts.end(); ++script) {
						if (*script != nullptr) {
							to_save->SetAnotherNode();
							to_save->SetString("GameObjectID", std::to_string((*item)->ID).data());
							to_save->SetString("ResourceScriptID", std::to_string((*script)->resourceID).data());
							to_save->SetString("DataName", (*script)->data_name.data());
							to_save->SetString("CompScriptID", std::to_string((*script)->ID).data());
							if ((*script)->inspector_variables.empty()) {
								to_save->SetBoolean("HasInspector", false);
							}
							else {
								to_save->SetBoolean("HasInspector", true);
								JSONArraypack* inspector = to_save->InitNewArray("Inspector");
								for (uint i = 0; i < (*script)->inspector_variables.size(); ++i) {
									inspector->SetAnotherNode();
									if ((*script)->inspector_variables[i].ptr == nullptr && (*script)->inspector_variables[i].obj == nullptr) {
										inspector->SetBoolean("IsNull", true);
										continue;
									}
									inspector->SetBoolean("IsNull", false);
									inspector->SetString("Name", (*script)->inspector_variables[i].variable_name.data());
									inspector->SetNumber("Type", (*script)->inspector_variables[i].variable_type);
									switch ((*script)->inspector_variables[i].variable_type)
									{
									case InspectorScriptData::DataType::INT: {
										inspector->SetNumber("int", (*(int*)((*script)->inspector_variables[i].ptr)));
										break; }
									case InspectorScriptData::DataType::ENUM: {
										inspector->SetNumber("enumInt", (*(int*)((*script)->inspector_variables[i].ptr)));
										break; }
									case InspectorScriptData::DataType::STRING: {
										inspector->SetString("string", ((std::string*)((*script)->inspector_variables[i].ptr))->data());
										break; }
									case InspectorScriptData::DataType::FLOAT: {
										inspector->SetNumber("float", (*(float*)((*script)->inspector_variables[i].ptr)));
										break; }
									case InspectorScriptData::DataType::BOOL: {
										inspector->SetNumber("bool", (*(bool*)((*script)->inspector_variables[i].ptr)));
										break; }
									case InspectorScriptData::DataType::PREFAB: {
										Prefab* prefab = ((Prefab*)((*script)->inspector_variables[i].ptr));
										inspector->SetString("prefab", std::to_string(prefab->prefabID).data());
										break; }
									case InspectorScriptData::DataType::GAMEOBJECT: {
										GameObject** obj = ((GameObject**)((*script)->inspector_variables[i].obj));
										if (obj != nullptr && *obj != nullptr) {
											inspector->SetString("gameobject", std::to_string((*obj)->ID).data());
										}
										else {
											inspector->SetString("gameobject", "0");
										}
										break; }
									default:
										break;
									}
								}
							}
							(*item)->RemoveComponent(*script);
						}
					}
				}
				CreateJsonScript((*item), to_save);
			}
		}
	}
}

void ModuleObjects::ReAssignScripts(JSONArraypack* to_load)
{
	for (uint i = 0; i < to_load->GetArraySize(); ++i) {
		GameObject* obj = GetGameObjectByID(std::stoull(to_load->GetString("GameObjectID")));
		if (obj == nullptr) {
			to_load->GetAnotherNode();
			continue;
		}
		ResourceScript* r_script = (ResourceScript*)App->resources->GetResourceWithID(std::stoull(to_load->GetString("ResourceScriptID")));
		if (r_script == nullptr) {
			to_load->GetAnotherNode();
			continue;
		}
		std::string data_name = to_load->GetString("DataName");
		for (uint k = 0; k < r_script->data_structures.size(); ++k) {
			if (App->StringCmp(data_name.data(), r_script->data_structures[k].first.data())) {
				ComponentScript* script = new ComponentScript(obj);
				script->ID = std::stoull(to_load->GetString("CompScriptID"));
				script->resourceID = r_script->GetID();
				script->LoadData(data_name.data(), r_script->data_structures[k].second);
				if (to_load->GetBoolean("HasInspector")) {
					JSONArraypack* inspector = to_load->GetArray("Inspector");
					for (uint j = 0; j < inspector->GetArraySize(); ++j) {
						if (inspector->GetBoolean("IsNull")) {
							inspector->GetAnotherNode();
							continue;
						}
						std::vector<InspectorScriptData>::iterator item = script->inspector_variables.begin();
						std::string var_name = inspector->GetString("Name");
						InspectorScriptData::DataType type = (InspectorScriptData::DataType)(uint)inspector->GetNumber("Type");
						for (; item != script->inspector_variables.end(); ++item) {
							if (App->StringCmp((*item).variable_name.data(), var_name.data())) {
								if (type == (*item).variable_type) {
									switch (type) {
									case InspectorScriptData::DataType::INT: {
										*(int*)(*item).ptr = inspector->GetNumber("int");
										break; }
									case InspectorScriptData::DataType::ENUM: {
										*(int*)(*item).ptr = inspector->GetNumber("enumInt");
										break; }
									case InspectorScriptData::DataType::FLOAT: {
										*(float*)(*item).ptr = inspector->GetNumber("float");
										break; }
									case InspectorScriptData::DataType::STRING: {
										std::string* data = (std::string*)(*item).ptr;
										void (*Add)(std::string*, const char*) = (void (*)(std::string*, const char*))GetProcAddress(App->scripts_dll, std::string("ChangeString").data());
										Add(data, inspector->GetString("string"));
										break; }
									case InspectorScriptData::DataType::BOOL: {
										*(bool*)(*item).ptr = inspector->GetNumber("bool");
										break; }
									case InspectorScriptData::DataType::PREFAB: {
										u64 id = std::stoull(inspector->GetString("prefab"));
										if (id != 0) {
											ResourcePrefab* prefab = (ResourcePrefab*)App->resources->GetResourceWithID(id);
											if (prefab != nullptr) {
												Prefab* ins_prefab = (Prefab*)(*item).ptr;
												ins_prefab->prefabID = prefab->GetID();
												ins_prefab->prefab_name = std::string(prefab->GetName());
												prefab->prefab_references.push_back(ins_prefab);
											}
										}
										break; }
									case InspectorScriptData::DataType::GAMEOBJECT: {
										u64 id = std::stoull(inspector->GetString("gameobject"));
										if (id != 0) {
											*(*item).obj = GetGameObjectByID(id);
										}
										break; }
									default:
										break;
									}
								}
							}
						}
						inspector->GetAnotherNode();
					}
				}
			}
		}
		to_load->GetAnotherNode();
	}
	ReAttachUIScriptEvents();
}

void ModuleObjects::DeleteReturns()
{
	if (!fordward_actions.empty()) {
		while (!fordward_actions.empty()) {
			ReturnZ* act = fordward_actions.top();
			delete act;
			act = nullptr;
			fordward_actions.pop();
		}
	}
	if (!return_actions.empty()) {
		while (!return_actions.empty()) {
			ReturnZ* act = return_actions.top();
			delete act;
			act = nullptr;
			return_actions.pop();
		}
	}
}

void ModuleObjects::UpdateGamePadInput()
{
	if (GetGameObjectByID(selected_ui) != nullptr && GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>() != nullptr && GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state != Pressed && GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->canvas != nullptr && GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->canvas->allow_navigation && GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->active)
	{
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_DPAD_UP) || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || /*Input::GetControllerVerticalLeftAxis(1) > 0.2f*/ Input::GetControllerJoystickLeft(1, Input::JOYSTICK_BUTTONS::JOYSTICK_UP) == KEY_DOWN)
		{
			if (GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_up != -1)
			{
				u64 neightbour_temp = GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_up;
				if (GetGameObjectByID(neightbour_temp) != nullptr)
				{
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Exit; //put state exit
					u64 safe_selected = selected_ui;
					selected_ui = SetNewSelected("up", GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_up);
					if (selected_ui == -1)
						selected_ui = safe_selected;
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Enter;
				}
			}
		}
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_DPAD_DOWN) || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || /*Input::GetControllerVerticalLeftAxis(1) < -0.2f*/ Input::GetControllerJoystickLeft(1, Input::JOYSTICK_BUTTONS::JOYSTICK_DOWN) == KEY_DOWN)
		{
			if (GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_down != -1)
			{
				u64 neightbour_temp = GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_down;
				if (GetGameObjectByID(neightbour_temp) != nullptr)
				{
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Exit;
					u64 safe_selected = selected_ui;
					selected_ui = SetNewSelected("down", GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_down);
					if (selected_ui == -1)
						selected_ui = safe_selected;
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Enter;
				}
			}
		}
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_DPAD_RIGHT) || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || /*Input::GetControllerHoritzontalLeftAxis(1) < -0.2f*/ Input::GetControllerJoystickLeft(1, Input::JOYSTICK_BUTTONS::JOYSTICK_RIGHT) == KEY_DOWN)
		{
			if (GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_right != -1)
			{
				u64 neightbour_temp = GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_right;
				if (GetGameObjectByID(neightbour_temp) != nullptr)
				{
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Exit;
					u64 safe_selected = selected_ui;
					selected_ui = SetNewSelected("right", GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_right);
					if (selected_ui == -1)
						selected_ui = safe_selected;
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Enter;
				}
			}
		}
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_DPAD_LEFT) || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || /*Input::GetControllerHoritzontalLeftAxis(1) > 0.2f*/ Input::GetControllerJoystickLeft(1, Input::JOYSTICK_BUTTONS::JOYSTICK_LEFT) == KEY_DOWN)
		{
			if (GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_left != -1)
			{
				u64 neightbour_temp = GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_left;
				if (GetGameObjectByID(neightbour_temp) != nullptr)
				{
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Exit;
					u64 safe_selected = selected_ui;
					selected_ui = SetNewSelected("left", GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->select_on_left);
					if (selected_ui == -1)
						selected_ui = safe_selected;
					GetGameObjectByID(selected_ui)->GetComponent<ComponentUI>()->state = Enter;
				}
			}
		}
	}
}

u64 ModuleObjects::SetNewSelected(std::string neightbour, u64 selected_neightbour)
{
	if (neightbour == "up")
	{
		if (GetGameObjectByID(selected_neightbour)->enabled)
			return selected_neightbour;

		else
		{
			if (GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_up != -1)
				return SetNewSelected("up", GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_up);
			else
				return -1;
		}
	}
	else if (neightbour == "down")
	{
		if (GetGameObjectByID(selected_neightbour)->enabled)
			return selected_neightbour;

		else
		{
			if (GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_down != -1)
				return SetNewSelected("down", GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_down);
			else
				return -1;
		}
	}
	else if (neightbour == "right")
	{
		if (GetGameObjectByID(selected_neightbour)->enabled)
			return selected_neightbour;

		else
		{
			if (GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_right != -1)
				return SetNewSelected("right", GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_right);
			else
				return -1;
		}
	}
	else if (neightbour == "left")
	{
		if (GetGameObjectByID(selected_neightbour)->enabled)
			return selected_neightbour;

		else
		{
			if (GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_left != -1)
				return SetNewSelected("left", GetGameObjectByID(selected_neightbour)->GetComponent<ComponentUI>()->select_on_left);
			else
				return -1;
		}
	}
	else
	{
		LOG_ENGINE("Something went wrong");
		return -1;
	}
}

ComponentCanvas* ModuleObjects::GetCanvas()
{
	ComponentCanvas* canvas = GetRoot(true)->GetCanvas();
	if (canvas == nullptr) {
		GameObject* obj = new GameObject(GetRoot(false));
		obj->SetName("Canvas");
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}

void ModuleObjects::UIOrdering(std::vector<std::pair<float, GameObject*>>* current, std::vector<std::pair<float, GameObject*>>* ui_2d, std::vector<std::pair<float, GameObject*>>* ui_world)
{
	std::vector<std::pair<float, GameObject*>>::iterator it_ui = current->begin();
	for (; it_ui != current->end(); ++it_ui)
	{
		if ((*it_ui).second != nullptr)
		{
			ComponentUI* ui = (*it_ui).second->GetComponent<ComponentUI>();
			if (ui->canvas != nullptr && !ui->canvas->isWorld)
				ui_2d->push_back((*it_ui));

			else if (ui->canvas != nullptr && ui->canvas->isWorld)
				ui_world->push_back((*it_ui));
		}
	}

#ifndef GAME_VERSION
	std::sort(ui_2d->begin(), ui_2d->end(), ModuleObjects::SortUIToDraw);
	if (!printing_scene) {
		std::sort(ui_2d->begin(), ui_2d->end(), ModuleObjects::SortGameObjectToDraw);
	}

	std::sort(ui_world->begin(), ui_world->end(), ModuleObjects::SortUIToDraw);
#else
	std::sort(ui_2d->begin(), ui_2d->end(), ModuleObjects::SortGameObjectToDraw);
	std::sort(ui_world->begin(), ui_world->end(), ModuleObjects::SortUIToDraw);

#endif

}



void ModuleObjects::CompareName(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::vector<ComponentScript*>& scriptsVec)
{
	auto item = listeners->begin();
	bool skip = false;
	for (; item != listeners->end(); ++item) {
		for (auto scripts = scriptsVec.begin(); scripts != scriptsVec.end(); ++scripts) {
			for (auto funct = (*scripts)->functionMap.begin(); funct != (*scripts)->functionMap.end(); ++funct) {
				if ((*funct).first == (*item).first) {
					(*item).second = ([funct]() -> void { (*funct).second(); });
					skip = true;
					break;
				}
			}
			if (skip) {
				skip = false;
				break;
			}
		}
	}
	item = listeners->begin();
	while (item != listeners->end()) {
		if ((*item).second == nullptr) {
			item = listeners->erase(item);
		}
		else {
			++item;
		}
	}
}

bool ModuleObjects::SortByFamilyNumber(std::tuple<uint, u64, uint> tuple1, std::tuple<uint, u64, uint> tuple2)
{
	return std::get<0>(tuple1) < std::get<0>(tuple2);
}

void ModuleObjects::LoadConfig(JSONfilepack*& config)
{
	wireframe_mode = config->GetBoolean("Configuration.Renderer.Wireframe");
	view_mesh_mode = config->GetBoolean("Configuration.Renderer.MeshView");
	draw_vertex_normals = config->GetBoolean("Configuration.Renderer.VertexNormals");
	draw_face_normals = config->GetBoolean("Configuration.Renderer.FaceNormals");
	mesh_line_width = config->GetNumber("Configuration.Renderer.MeshWidth");
	face_n_width = config->GetNumber("Configuration.Renderer.FaceNormalsWidth");
	vertex_n_width = config->GetNumber("Configuration.Renderer.VertexNormalsWidth");
	vertex_normal_length = config->GetNumber("Configuration.Renderer.VertexNormalsLength");
	face_normal_length = config->GetNumber("Configuration.Renderer.FaceNormalsLength");
	allow_grid = config->GetBoolean("Configuration.Renderer.AllowGrid");
	App->renderer3D->grid_spacing = config->GetNumber("Configuration.Renderer.GridSpacing");
	App->renderer3D->length_grid = config->GetNumber("Configuration.Renderer.GridLength");
	App->renderer3D->line_grid_width = config->GetNumber("Configuration.Renderer.GridWidth");
	App->renderer3D->grid_color = config->GetColor("Configuration.Renderer.GridColor");
	vertex_n_color = config->GetColor("Configuration.Renderer.VertexNormalColor");
	face_n_color = config->GetColor("Configuration.Renderer.FaceNormalColor");
	mesh_color = config->GetColor("Configuration.Renderer.MeshColor");
	parent_outline_color = config->GetColor("Configuration.Renderer.ParentOutlineColor");
	no_child_outline_color = config->GetColor("Configuration.Renderer.NoChildOutlineColor");
	outline = config->GetBoolean("Configuration.Renderer.Outline");
	parent_line_width = config->GetNumber("Configuration.Renderer.ParentLineWidth");
	no_child_line_width = config->GetNumber("Configuration.Renderer.NoChildLineWidth");
	draw_all_AABB = config->GetBoolean("Configuration.Renderer.DrawAABB");
	draw_all_OBB = config->GetBoolean("Configuration.Renderer.DrawOBB");
	global_AABB_color = config->GetColor("Configuration.Renderer.ColorAABB");
	global_OBB_color = config->GetColor("Configuration.Renderer.ColorOBB");
	AABB_line_width = config->GetNumber("Configuration.Renderer.LineWidthAABB");
	OBB_line_width = config->GetNumber("Configuration.Renderer.LineWidth0BB");
	frustum_color = config->GetColor("Configuration.Renderer.FrustumColor");
	frustum_line_width = config->GetNumber("Configuration.Renderer.FrustumLineWidth");
	draw_frustum = config->GetBoolean("Configuration.Renderer.DrawFrustum");
	octree.SetBucket(config->GetNumber("Configuration.Renderer.Octreebucket"));
	octree.Recalculate(nullptr);
	octree_line_color = config->GetColor("Configuration.Renderer.OctreeColor");
	octree_line_width = config->GetNumber("Configuration.Renderer.OctreelineWidth");
	render_octree = config->GetBoolean("Configuration.Renderer.RenderOctree");
	draw_ray = config->GetBoolean("Configuration.Renderer.DrawRay");
	ray_width = config->GetNumber("Configuration.Renderer.RayWidth");
	ray_color = config->GetColor("Configuration.Renderer.RayColor");
}

void ModuleObjects::SaveConfig(JSONfilepack*& config)
{
	config->SetBoolean("Configuration.Renderer.Wireframe", wireframe_mode);
	config->SetBoolean("Configuration.Renderer.MeshView", view_mesh_mode);
	config->SetBoolean("Configuration.Renderer.VertexNormals", draw_vertex_normals);
	config->SetBoolean("Configuration.Renderer.FaceNormals", draw_face_normals);
	config->SetNumber("Configuration.Renderer.MeshWidth", mesh_line_width);
	config->SetNumber("Configuration.Renderer.FaceNormalsWidth", face_n_width);
	config->SetNumber("Configuration.Renderer.VertexNormalsWidth", vertex_n_width);
	config->SetNumber("Configuration.Renderer.VertexNormalsLength", vertex_normal_length);
	config->SetNumber("Configuration.Renderer.FaceNormalsLength", face_normal_length);
	config->SetBoolean("Configuration.Renderer.AllowGrid", allow_grid);
	config->SetNumber("Configuration.Renderer.GridSpacing", App->renderer3D->grid_spacing);
	config->SetNumber("Configuration.Renderer.GridWidth", App->renderer3D->line_grid_width);
	config->SetNumber("Configuration.Renderer.GridLength", App->renderer3D->length_grid);
	config->SetColor("Configuration.Renderer.GridColor", App->renderer3D->grid_color);
	config->SetColor("Configuration.Renderer.VertexNormalColor", vertex_n_color);
	config->SetColor("Configuration.Renderer.FaceNormalColor", face_n_color);
	config->SetColor("Configuration.Renderer.MeshColor", mesh_color);
	config->SetColor("Configuration.Renderer.ParentOutlineColor", parent_outline_color);
	config->SetColor("Configuration.Renderer.NoChildOutlineColor", no_child_outline_color);
	config->SetBoolean("Configuration.Renderer.Outline", outline);
	config->SetNumber("Configuration.Renderer.ParentLineWidth", parent_line_width);
	config->SetNumber("Configuration.Renderer.NoChildLineWidth", no_child_line_width);
	config->SetBoolean("Configuration.Renderer.DrawAABB", draw_all_AABB);
	config->SetBoolean("Configuration.Renderer.DrawOBB", draw_all_OBB);
	config->SetColor("Configuration.Renderer.ColorAABB", global_AABB_color);
	config->SetColor("Configuration.Renderer.ColorOBB", global_OBB_color);
	config->SetNumber("Configuration.Renderer.LineWidthAABB", AABB_line_width);
	config->SetNumber("Configuration.Renderer.LineWidthOBB", OBB_line_width);
	config->SetNumber("Configuration.Renderer.FrustumLineWidth", frustum_line_width);
	config->SetColor("Configuration.Renderer.FrustumColor", frustum_color);
	config->SetBoolean("Configuration.Renderer.DrawFrustum", draw_frustum);
	config->SetBoolean("Configuration.Renderer.RenderOctree", render_octree);
	config->SetNumber("Configuration.Renderer.OctreelineWidth", octree_line_width);
	config->SetColor("Configuration.Renderer.OctreeColor", octree_line_color);
	config->SetNumber("Configuration.Renderer.Octreebucket", octree.GetBucket());
	config->SetBoolean("Configuration.Renderer.DrawRay", draw_ray);
	config->SetNumber("Configuration.Renderer.RayWidth", ray_width);
	config->SetColor("Configuration.Renderer.RayColor", ray_color);
}

void ModuleObjects::HandleAlienEvent(const AlienEvent& alien_event)
{
	std::stack<GameObject*> go_stack;

	if (base_game_object)
	{
		go_stack.push(base_game_object);
	}

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

void ModuleObjects::HandleEvent(EventType eventType)
{
	std::vector<GameObject*> objects;
	objects.push_back(base_game_object);

	while (!objects.empty())
	{
		GameObject* currentGo = objects.back();
		objects.pop_back();
		objects.insert(objects.end(), currentGo->children.begin(), currentGo->children.end());

		switch (eventType)
		{
		case EventType::ON_PLAY:
			currentGo->OnPlay();
			break;
		case EventType::ON_PAUSE:
			currentGo->OnPause();
			break;
		case EventType::ON_STOP:
			currentGo->OnStop();
			break;
		}
	}
	objects.clear();

	if (eventType == EventType::ON_PLAY) {
		InitScripts();
	}
	else if (eventType == EventType::ON_ASSET_DELETE) {
#ifndef GAME_VERSION
		App->ui->panel_project->selected_resource = nullptr;
#endif
	}


}

void ModuleObjects::CreateBasePrimitive(PrimitiveType type)
{
	GameObject* object = new GameObject(GetRoot(false));
	ResourceMesh* resource = nullptr;
	ComponentMesh* mesh = new ComponentMesh(object);
	ComponentMaterial* material = new ComponentMaterial(object);
	ComponentCollider* collider = nullptr;

	switch (type) {
	case PrimitiveType::CUBE: {
		resource = App->resources->GetPrimitive(PrimitiveType::CUBE);
		object->SetName("Cube");
		break; }
	case PrimitiveType::DODECAHEDRON: {
		resource = App->resources->GetPrimitive(PrimitiveType::DODECAHEDRON);
		object->SetName("Dodecahedron");
		break; }
	case PrimitiveType::ICOSAHEDRON: {
		resource = App->resources->GetPrimitive(PrimitiveType::ICOSAHEDRON);
		object->SetName("Icosahedron");
		break; }
	case PrimitiveType::OCTAHEDRON: {
		resource = App->resources->GetPrimitive(PrimitiveType::OCTAHEDRON);
		object->SetName("Octahedron");
		break; }
	case PrimitiveType::ROCK: {
		resource = App->resources->GetPrimitive(PrimitiveType::ROCK);
		object->SetName("Rock");
		break; }
	case PrimitiveType::SPHERE_ALIEN: {
		resource = App->resources->GetPrimitive(PrimitiveType::SPHERE_ALIEN);
		object->SetName("Sphere");
		break; }
	case PrimitiveType::TORUS: {
		resource = App->resources->GetPrimitive(PrimitiveType::TORUS);
		object->SetName("Torus");
		break; }
	default:
		break;
	}

	object->AddComponent(mesh);
	object->AddComponent(material);
	mesh->SetResourceMesh(resource);

	// Add collider --------------------------------------------

	switch (type) {
	case PrimitiveType::CUBE: {
		collider = new ComponentBoxCollider(object);
		break; }
	case PrimitiveType::SPHERE_ALIEN: {
		collider = new ComponentSphereCollider(object);
		break; }
	}

	if (collider != nullptr)
	{
		object->AddComponent(collider);
	}
	// ---------------------------------------------------------


	SetNewSelectedObject(object, false);
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, object);
}

void ModuleObjects::CreateBaseUI(ComponentType type)
{
	GameObject* object = CreateEmptyGameObject(nullptr, false);
	Component* comp = nullptr;
	Component* comp_emitter = nullptr;
	Component* comp_text = nullptr;

	switch (type)
	{
	case ComponentType::CANVAS: {
		object->SetName("Canvas");
		comp = new ComponentCanvas(object);
		object->AddComponent(comp);
		break; }

	case ComponentType::UI_IMAGE: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentImage(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Image");
		object->AddComponent(comp);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }

	case ComponentType::UI_BUTTON: {
		GameObject* object_text = CreateEmptyGameObject(nullptr);

		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentButton(object);
		comp_emitter = new ComponentAudioEmitter(object);

		comp_text = new ComponentText(object_text);

		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		dynamic_cast<ComponentUI*>(comp_text)->SetCanvas(canvas);

		object->SetName("Button");
		object->AddComponent(comp);
		object->AddComponent(comp_emitter);

		object_text->SetName("Text");
		object_text->AddComponent(comp_text);



		ReparentGameObject(object, canvas->game_object_attached, false);
		ReparentGameObject(object_text, object, false);
		break; }

	case ComponentType::UI_TEXT: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentText(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Text");
		object->AddComponent(comp);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }

	case ComponentType::UI_CHECKBOX: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentCheckbox(object);
		comp_emitter = new ComponentAudioEmitter(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Checkbox");
		object->AddComponent(comp);
		object->AddComponent(comp_emitter);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }

	case ComponentType::UI_SLIDER: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentSlider(object);
		comp_emitter = new ComponentAudioEmitter(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Slider");
		object->AddComponent(comp);
		object->AddComponent(comp_emitter);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }

	case ComponentType::UI_BAR: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentBar(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Bar");
		object->AddComponent(comp);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }
	case ComponentType::UI_ANIMATED_IMAGE: {
		ComponentCanvas* canvas = GetCanvas();
		comp = new ComponentAnimatedImage(object);
		dynamic_cast<ComponentUI*>(comp)->SetCanvas(canvas);
		object->SetName("Animated Image");
		object->AddComponent(comp);
		ReparentGameObject(object, canvas->game_object_attached, false);
		break; }

	default: {
		break; }
	}
	if (object != nullptr) {
		SetNewSelectedObject(object, false);
	}
}

void ModuleObjects::CreateLight(LightTypeObj type)
{
	GameObject* object = new GameObject(GetRoot(false));
	Component* comp = nullptr;

	switch (type)
	{
	case LightTypeObj::POINT:
	{
		object->SetName("Point light");
		comp = new ComponentLightPoint(object);
		object->AddComponent(comp);
		break;
	}
	case LightTypeObj::SPOT:
	{
		object->SetName("Spot light");
		comp = new ComponentLightSpot(object);
		object->AddComponent(comp);
		break;
	}
	case LightTypeObj::DIRECTIONAL:
	{
		object->SetName("Directional light");
		comp = new ComponentLightDirectional(object);
		object->AddComponent(comp);
		break;
	}
	default:
		break;
	}

	SetNewSelectedObject(object, false);
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, object);
}

void ModuleObjects::CreateEffect(ComponentType type)
{
	GameObject* object = new GameObject(GetRoot(false));
	Component* comp = nullptr;

	switch (type)
	{
	case ComponentType::PARTICLES:
	{
		object->SetName("Particle System");
		comp = new ComponentParticleSystem(object);
		object->AddComponent(comp);
		break;
	}
	default:
		break;
	}

	SetNewSelectedObject(object, false);
	ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, object);
}

uint ModuleObjects::GetNumOfPointLights() const
{
	return num_of_point_lights;
}

uint ModuleObjects::GetNumOfDirLights() const
{
	return num_of_dir_lights;
}

uint ModuleObjects::GetNumOfSpotLights() const
{
	return num_of_spot_lights;
}

void ModuleObjects::AddNumOfPointLights()
{
	++num_of_point_lights;
}

void ModuleObjects::AddNumOfDirLights()
{
	++num_of_dir_lights;
}

void ModuleObjects::AddNumOfSpotLights()
{
	++num_of_spot_lights;
}

void ModuleObjects::ReduceNumOfPointLights()
{
	--num_of_point_lights;
}

void ModuleObjects::ReduceNumOfDirLights()
{
	--num_of_dir_lights;
}

void ModuleObjects::ReduceNumOfSpotLights()
{
	--num_of_spot_lights;
}