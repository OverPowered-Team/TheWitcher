#include "Component.h"
#include "ComponentUI.h"
#include "Application.h"
#include "ModuleUI.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentTransform.h"
#include "ModuleWindow.h"
#include "ResourceTexture.h"
#include "ResourceFont.h"
#include "PanelGame.h"
#include "PanelScene.h"
#include "ModuleInput.h"
#include "ModuleObjects.h"
#include "ModuleRenderer3D.h"
#include "StaticInput.h"
#include "mmgr/mmgr.h"

ComponentUI::ComponentUI(GameObject* obj):Component(obj)
{
	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6 * 3, index, GL_STATIC_DRAW);

	type = ComponentType::UI;
}

ComponentUI::~ComponentUI()
{
	glDeleteBuffers(1, &uvID);
	glDeleteBuffers(1, &verticesID);
	glDeleteBuffers(1, &indexID);

	if (texture != nullptr) {
		texture->DecreaseReferences();
	}
}

void ComponentUI::ChangeVertex(float width, float height)
{
	size = { width, height };

	vertices[0] = { 0,0,0 };
	vertices[1] = { 0,-size.y,0 };
	vertices[2] = { size.x,-size.y,0 };
	vertices[3] = { size.x, 0,0 };

	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_DYNAMIC_DRAW);
}

void ComponentUI::UpdateVertex()
{
	vertices[0] = { -size.x,size.y,0 };
	vertices[1] = { -size.x, -size.y,0 };
	vertices[2] = { size.x, -size.y,0 };
	vertices[3] = { size.x, size.y,0 };

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_DYNAMIC_DRAW);
}

void ComponentUI::SetCanvas(ComponentCanvas* canvas_)
{
	if (canvas_ != nullptr) {
		canvas = canvas_;
		canvas_trans = canvas->game_object_attached->GetComponent<ComponentTransform>();
	}
	else {
		canvas = nullptr;
		canvas_trans = nullptr;
	}
}

void ComponentUI::Update()
{
	if (Time::IsPlaying()) {
		if (!App->objects->first_assigned_selected || (App->objects->GetGameObjectByID(App->objects->selected_ui) != nullptr && !App->objects->GetGameObjectByID(App->objects->selected_ui)->enabled))
			CheckFirstSelected();
		
		//UILogicMouse();

		switch (state)
		{
		case Idle:
			break;
		case Hover:
			OnHover();
			break;
		case Click:
			OnClick();
			break;
		case Pressed:
			OnPressed();
			break;
		case Release:
			OnRelease();
			break;
		default:
			break;
		}
		UILogicGamePad();
	}
}

void ComponentUI::Draw(bool isGame)
{
	if (canvas == nullptr || canvas_trans == nullptr) {
		return;
	}

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	
	if (isGame && App->renderer3D->actual_game_camera != nullptr) {

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		#ifndef GAME_VERSION
		glOrtho(0,App->ui->panel_game->width, App->ui->panel_game->height, 0, App->renderer3D->actual_game_camera->frustum.farPlaneDistance, App->renderer3D->actual_game_camera->frustum.farPlaneDistance);
		#else
		glOrtho(0, App->window->width, App->window->height, 0, App->renderer3D->actual_game_camera->frustum.farPlaneDistance, App->renderer3D->actual_game_camera->frustum.farPlaneDistance);
		#endif
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		matrix[0][0] /= canvas->width * 0.5F;
		matrix[1][1] /= canvas->height * 0.5F;
		float3 canvas_pos = canvas_trans->GetGlobalPosition();
		float3 object_pos = transform->GetGlobalPosition();
		float3 canvasPivot = { canvas_pos.x - canvas->width * 0.5F, canvas_pos.y + canvas->height * 0.5F, 0 };
		float2 origin = float2((object_pos.x - canvasPivot.x) / (canvas->width), (object_pos.y - canvasPivot.y) / (canvas->height));
		
		#ifndef GAME_VERSION
		x = origin.x * App->ui->panel_game->width;
		y = -origin.y * App->ui->panel_game->height;
		#else
		x = origin.x * App->window->width;
		y = origin.y * App->window->height;
		#endif
		
		origin.x = (origin.x - 0.5F) * 2;
		origin.y = -(-origin.y - 0.5F) * 2;
		matrix[0][3] = origin.x;
		matrix[1][3] = origin.y;
		matrix[2][3] = 0.0f;
	}

	if (texture != nullptr) {
		//glAlphaFunc(GL_GREATER, 0.0f);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	glPushMatrix();
	glMultMatrixf(matrix.Transposed().ptr());

	glEnableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, 0);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

void ComponentUI::ClearTexture()
{
	if (texture != nullptr) {
		texture->DecreaseReferences();
		texture = nullptr;
	}
}

void ComponentUI::SetTexture(ResourceTexture* tex)
{
	if (tex != nullptr && tex != texture) {
		tex->IncreaseReferences();
		if (texture != nullptr) {
			texture->DecreaseReferences();
		}
		texture = tex;
	}
}

bool ComponentUI::CheckMouseInside(float3 mouse_pos)
{
	ComponentTransform* trans = game_object_attached->GetComponent<ComponentTransform>();
#ifdef GAME_VERSION
	return (mouse_pos.x >= x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->window->width) * 0.5F && mouse_pos.x <= x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->window->width) * 0.5F && mouse_pos.y >= y - ((trans->global_transformation[1][1] / (canvas->height * 0.5F) * App->window->height) * 0.5F) && mouse_pos.y <= y + ((trans->global_transformation[1][1] / (canvas->height * 0.5F)) * App->window->height) * 0.5F);
#else
	return (mouse_pos.x >= x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F && mouse_pos.x <= x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F && mouse_pos.y >= y - ((trans->global_transformation[1][1] / (canvas->height * 0.5F) * App->ui->panel_game->height) * 0.5F) && mouse_pos.y <= y + ((trans->global_transformation[1][1] / (canvas->height * 0.5F)) * App->ui->panel_game->height) * 0.5F);
#endif
}

void ComponentUI::UILogicMouse()
{
	float3 mouse_pos;

	#ifndef GAME_VERSION
	mouse_pos = float3((App->input->GetMouseX() - App->ui->panel_game->posX), (App->input->GetMouseY() - App->ui->panel_game->posY ), App->input->GetMouseZ());
	LOG_ENGINE("%f", mouse_pos.y);
	LOG_ENGINE("MOUSE POS: %i", App->input->GetMouseY());
	#else
	mouse_pos = App->input->GetMousePosition();
	#endif

	switch (state)
	{
	case Idle:
		if (CheckMouseInside(mouse_pos))
			state = Hover;
		break;
	case Hover:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			state = Click;
		if (!CheckMouseInside(mouse_pos))
			state = Release;
		break;
	case Click:
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			state = Pressed;
		break;
	case Pressed:	
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && CheckMouseInside(mouse_pos))
			state = Hover;
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && !CheckMouseInside(mouse_pos))
			state = Idle;
		break;
	case Release:
		state = Idle;
		break;
	}
}

void ComponentUI::SetBackgroundColor(float r, float g, float b, float a)
{
	current_color = { r,g,b,a };
}

void ComponentUI::CheckFirstSelected()
{
	if (tabbable && this->game_object_attached != nullptr && this->game_object_attached->enabled)
	{
		if (App->objects->GetGameObjectByID(App->objects->selected_ui)!=nullptr &&  App->objects->selected_ui != -1)
			App->objects->GetGameObjectByID(App->objects->selected_ui)->GetComponent<ComponentUI>()->state = Release;

		App->objects->selected_ui = this->game_object_attached->ID;
		App->objects->GetGameObjectByID(App->objects->selected_ui)->GetComponent<ComponentUI>()->state = Hover;
		App->objects->first_assigned_selected = true;
	}
}



void ComponentUI::UILogicGamePad()
{
	switch (state)
	{
	case Idle: {
		//not necessary to do anything
		break; }
	case Hover: {
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_A) || App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
			state = Click;
		
		break; }
	case Click: {
		if (Input::GetControllerButtonRepeat(1, Input::CONTROLLER_BUTTON_A) || App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_REPEAT)
			state = Pressed;
		
		if (Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_A) || App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_UP)
			state = Hover;
		
		break; }
	case Pressed: {
		if (Input::GetControllerButtonUp(1, Input::CONTROLLER_BUTTON_A) || App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_UP)
			state = Hover;
		
		break; }
	case Release: {
		state = Idle;
		break; }
	}
}
