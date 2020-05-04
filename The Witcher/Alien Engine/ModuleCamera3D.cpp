#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "ModuleObjects.h"
#include "ModuleUI.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "PanelScene.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "Viewport.h"
#include "ComponentCurve.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name.assign("Camera3D");
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(fake_camera);
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG_ENGINE("Setting up the camera");
	bool ret = true;
	scene_viewport = new Viewport(fake_camera);
	selected_viewport = new Viewport(nullptr);
	App->camera->fake_camera->frustum.pos = { 25,25,25 };
	final_yaw = current_yaw = 225;
	final_pitch = current_pitch = 30;
	Rotate(current_yaw, current_pitch);
	max_distance = 10;
	return ret;
}
// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG_ENGINE("Cleaning camera");

	SDL_FreeCursor(cursor);

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	OPTICK_EVENT();
	frustum = &fake_camera->frustum;

	speed = camera_speed * dt;
	zoom_speed = camera_zoom_speed * dt;
	

	mouse_motion_x = -App->input->GetMouseXMotion();
	mouse_motion_y = App->input->GetMouseYMotion();

	if (start_lerp)
	{
		fake_camera->frustum.pos = Lerp(fake_camera->frustum.pos, point_to_look, 0.5f);
		if (fake_camera->frustum.pos.Equals(point_to_look))
		{
			start_lerp = false;
		}
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			speed = camera_speed * 2.f * dt;
			zoom_speed = camera_zoom_speed * 2.f * dt;
		}
		if (is_scene_hovered)
		{
			Zoom();
			if ((App->objects->GetSelectedObjects().empty() || (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver())) && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) 
			{
				CreateRay();
			}

			if (!ImGuizmo::IsUsing()) 
			{
				Movement(dt);
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
		{
			std::list<GameObject*> selected = App->objects->GetSelectedObjects();

			if (selected.size() > 0)
			{
				float4x4 trans = float4x4::zero();
				for (auto item = selected.begin(); item != selected.end(); ++item)
				{
					if (*item != nullptr)
					{
						trans += (*item)->transform->GetGlobalMatrix();
					}
				}
				trans /= selected.size();
				reference = trans.TranslatePart();
			}
			else
			{
				reference = fake_camera->frustum.pos + (fake_camera->frustum.front) * (fake_camera->frustum.pos - reference).Length();
			}
		}

		if (!ImGuizmo::IsUsing() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && is_scene_hovered)
		{
			
			if(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
				Orbit(dt);
				
			else
			{
				Rotation(dt);
			}
			
		}
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
			Focus();
		}

		if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		{
			float3 to_look(0, 0, 0);
			if (!App->objects->GetSelectedObjects().empty())
			{
				auto item = App->objects->GetSelectedObjects().begin();
				for (; item != App->objects->GetSelectedObjects().end(); ++item) {
					if (*item != nullptr) {
						to_look += (*item)->GetBB().CenterPoint();
					}
				}
				to_look /= App->objects->GetSelectedObjects().size();
			}
			fake_camera->Look(to_look);
			reference = to_look;
		}
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	frustum->Translate(Movement);
}

// -----------------------------------------------------------------
void ModuleCamera3D::Movement(float dt)
{
	OPTICK_EVENT();
	float3 movement(float3::zero());

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT) movement += float3::unitY();
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT) movement -= float3::unitY();

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) movement -= frustum->front;
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) movement += frustum->front;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) movement -= frustum->WorldRight();
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) movement += frustum->WorldRight();

		if (!movement.Equals(float3::zero()))
		{
			frustum->Translate(movement * speed);
			reference += movement*speed;
		}
	}

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		movement = float3::zero();
		float palm_speed = 10.f * dt;

		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		SDL_SetCursor(cursor);

		movement -= frustum->WorldRight() * palm_speed * App->input->GetMouseXMotion();
		movement += frustum->up * palm_speed * App->input->GetMouseYMotion();

		if (!movement.Equals(float3::zero()))
		{
			frustum->pos += movement;
			reference += movement;
		}

		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_UP)
		{
			SDL_FreeCursor(cursor);
		}
	}
}

void ModuleCamera3D::Zoom()
{
	float3 zoom(float3::zero());

	if (App->input->GetMouseZ() > 0)
	{
		zoom += frustum->front;
	}
	else if (App->input->GetMouseZ() < 0)
	{
		zoom -= frustum->front;
	}

	frustum->Translate(zoom * zoom_speed);
}

void ModuleCamera3D::Rotation(float dt)
{	
	float rotation_speed = camera_rotation_speed * dt;

	final_yaw += mouse_motion_x * rotation_speed;
	final_pitch += mouse_motion_y * rotation_speed;

	current_yaw = math::Lerp(current_yaw, final_yaw, lerp_rot_speed * dt);
	current_pitch = math::Lerp(current_pitch, final_pitch, lerp_rot_speed * dt);

	Rotate(current_yaw, current_pitch);

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	SDL_SetCursor(cursor);

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		SDL_FreeCursor(cursor);
	}
}

void ModuleCamera3D::Focus()
{
	if (!App->objects->GetSelectedObjects().empty())
	{
		AABB bounding_box;
		bounding_box.SetNegativeInfinity();
		auto item = App->objects->GetSelectedObjects().begin();

		for (; item != App->objects->GetSelectedObjects().end(); ++item) {
			if (*item != nullptr) {
				AABB obj_aabb = (*item)->GetBB();
				if (obj_aabb.IsFinite()) {
					bounding_box.maxPoint = { max(obj_aabb.maxPoint.x, bounding_box.maxPoint.x), max(obj_aabb.maxPoint.y, bounding_box.maxPoint.y), max(obj_aabb.maxPoint.z, bounding_box.maxPoint.z) };
					bounding_box.minPoint = { min(obj_aabb.minPoint.x, bounding_box.minPoint.x), min(obj_aabb.minPoint.y, bounding_box.minPoint.y), min(obj_aabb.minPoint.z, bounding_box.minPoint.z) };
				}
			}
		}

		float offset = bounding_box.Diagonal().Length();

		fake_camera->Look(bounding_box.CenterPoint());
		reference = bounding_box.CenterPoint();
		float3 vector_distance = fake_camera->frustum.pos - bounding_box.CenterPoint();

		point_to_look = fake_camera->frustum.pos - (vector_distance - (offset * vector_distance.Normalized()));
		start_lerp = true;
	}
}

void ModuleCamera3D::CreateRay()
{
	if (App->objects->GetGlobalRoot()->children.empty())
		return;

	float2 origin = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

	if (!scene_viewport->ScreenPointToViewport(origin))
		return;

	ray = scene_viewport->GetCamera()->frustum.UnProjectLineSegment(origin.x, origin.y);

	std::vector<std::pair<float, GameObject*>> hits;

	// with octree to static objects
	CreateObjectsHitMap(&hits, App->objects->octree.root, ray);
	
	// without octree for the dynamics
	std::vector<GameObject*>::iterator item = App->objects->GetGlobalRoot()->children.begin();
	for (; item != App->objects->GetGlobalRoot()->children.end(); ++item) {
		if (*item != nullptr && (*item)->IsEnabled()) {
			CreateObjectsHitMap(&hits, (*item), ray);
		}
	}
	// sort by pos
	std::sort(hits.begin(), hits.end(), ModuleCamera3D::SortByDistance);
	static bool hit = false;
	std::vector<std::pair<float, GameObject*>>::iterator it = hits.begin();
	for (; it != hits.end(); ++it) {
		if ((*it).second != nullptr) {
			if (TestTrianglesIntersections((*it).second, ray)) {
				hit = true;
				break;
			}
		}
	}

	if (!hit) {
		App->objects->DeselectObjects();
		App->ui->panel_scene->gizmo_curve = false;
		App->ui->panel_scene->curve = nullptr;
		App->ui->panel_scene->curve_index = 0;
	}

	hit = false;

}

void ModuleCamera3D::CreateObjectsHitMap(std::vector<std::pair<float, GameObject*>>* hits, GameObject* go, const LineSegment &ray)
{
	float distance_out = 0.f;
	float distance = 0.f;

	if (!go->is_static) {
		if (ray.Intersects(go->GetBB(), distance, distance_out))
		{
			hits->push_back({ distance, go });
		}
	}

	for (std::vector<GameObject*>::iterator iter = go->children.begin(); iter != go->children.end(); ++iter)
	{
		if ((*iter) != nullptr && (*iter)->IsEnabled())
			CreateObjectsHitMap(hits, (*iter), ray);
	}
}

void ModuleCamera3D::CreateObjectsHitMap(std::vector<std::pair<float, GameObject*>>* hits, OctreeNode* node, const LineSegment& ray)
{
	if (node != nullptr) {

		float distance_out = 0.f;
		float distance = 0.f;

		if (ray.Intersects(node->section, distance, distance_out))
		{
			if (!node->game_objects.empty()) {
				std::vector<GameObject*>::iterator item = node->game_objects.begin();
				for (; item != node->game_objects.end(); ++item) {
					if (*item != nullptr && (*item)->IsEnabled()) {
						if (ray.Intersects((*item)->GetBB(), distance, distance_out)) {
							hits->push_back({ distance, (*item) });
						}
					}
				}
			}

			if (!node->children.empty()) {
				std::vector<OctreeNode*>::iterator item = node->children.begin();
				for (; item != node->children.end(); ++item) {
					if (*item != nullptr) {
						CreateObjectsHitMap(hits, (*item), ray);
					}
				}
			}
		}
	}
}

bool ModuleCamera3D::TestTrianglesIntersections(GameObject* object, const LineSegment& ray)
{
	bool ret = false;
	ComponentMesh* mesh = (ComponentMesh*)object->GetComponent(ComponentType::MESH);

	if (mesh != nullptr && mesh->mesh != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)object->GetComponent(ComponentType::TRANSFORM);
		for (uint i = 0; i < mesh->mesh->num_index; i += 3)
		{
			uint index_a, index_b, index_c;

			index_a = mesh->mesh->index[i] * 3;
			float3 point_a(&mesh->mesh->vertex[index_a]);

			index_b = mesh->mesh->index[i + 1] * 3;
			float3 point_b(&mesh->mesh->vertex[index_b]);

			index_c = mesh->mesh->index[i + 2] * 3;
			float3 point_c(&mesh->mesh->vertex[index_c]);

			Triangle triangle_to_check(point_a, point_b, point_c);
			triangle_to_check.Transform(transform->global_transformation);
			if (ray.Intersects(triangle_to_check, nullptr, nullptr))
			{
				GameObject* obj = object->parent;
				while (obj->parent != nullptr) {
					obj->open_node = true;
					obj = obj->parent;
				}
				App->objects->SetNewSelectedObject(object, false);
				App->ui->panel_scene->gizmo_curve = false;
				App->ui->panel_scene->curve = nullptr;
				App->ui->panel_scene->curve_index = 0;
				ret = true;
				break;
			}
		}
	}
	else if (object->children.empty()){
		ComponentCurve* curve = object->GetComponent<ComponentCurve>();
		if (curve == nullptr) {
			App->objects->SetNewSelectedObject(object, false);
			App->ui->panel_scene->gizmo_curve = false;
			App->ui->panel_scene->curve = nullptr;
			App->ui->panel_scene->curve_index = 0;
			ret = true;
		}
		else {
			for (uint i = 0; i < curve->curve.GetControlPoints().size(); ++i) {
				if (ray.Intersects(AABB::FromCenterAndSize(curve->curve.GetControlPoints()[i], { 1,1,1 }))) {
					App->ui->panel_scene->gizmo_curve = true;
					App->ui->panel_scene->curve = curve;
					App->ui->panel_scene->curve_index = i;
					ret = true;
					App->objects->SetNewSelectedObject(object, false);
					break;
				}
			}
		}
	}
	return ret;
}

bool ModuleCamera3D::SortByDistance(const std::pair<float, GameObject*> pair1, const std::pair<float, GameObject*> pair2)
{
	return pair1.first < pair2.first;
}

void ModuleCamera3D::PanelConfigOption()
{
	if (fake_camera)
	{
		
		ImGui::SliderFloat("Far Plane", &fake_camera->frustum.farPlaneDistance, 100, 100000);
	}
}

void ModuleCamera3D::Rotate(float yaw, float pitch)
{
	Quat rotation = Quat::RotateAxisAngle(float3::unitY(), DegToRad(yaw)) * Quat::RotateAxisAngle(float3::unitX(), DegToRad(pitch));

	fake_camera->frustum.up = rotation.WorldY();
	fake_camera->frustum.front = rotation.WorldZ();
	
}

void ModuleCamera3D::Orbit(float dt)
{
	float rotation_speed =	camera_orbit_speed * dt;
	float yaw = mouse_motion_x * rotation_speed;
	float pitch = mouse_motion_y * rotation_speed;
	float distance = (fake_camera->frustum.pos - reference).Length();
	current_yaw = final_yaw += yaw;
	current_pitch = final_pitch += pitch;
	Rotate(current_yaw, current_pitch);
	fake_camera->frustum.pos = reference + -fake_camera->frustum.front * distance;
}



