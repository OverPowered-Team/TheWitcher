#include "Application.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCharacterController.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ReturnZ.h"

#include "Time.h"
#include "ModuleInput.h"
#include "mmgr/mmgr.h"

ComponentCharacterController::ComponentCharacterController(GameObject* go) : Component(go)
{
	type = ComponentType::CHARACTER_CONTROLLER;
	// GameObject Components 
	transform = game_object_attached->GetComponent<ComponentTransform>();

	shape = new btCapsuleShape(character_radius, character_height);

	body = new btPairCachingGhostObject();
	body->setCollisionShape(shape);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + character_offset, transform->GetGlobalRotation()));

	controller = new btKinematicCharacterController(body, (btConvexShape*)shape, 0.5);
	controller->setUp(btVector3(0.f, 1.f, 0.f));
	controller->setGravity(btVector3(0.f, -gravity, 0.f));
	jump_speed = controller->getJumpSpeed();

	App->physics->world->addCollisionObject(body, btBroadphaseProxy::CharacterFilter | btBroadphaseProxy::StaticFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter | btBroadphaseProxy::DefaultFilter);
	App->physics->AddAction(controller);

	detector = new btPairCachingGhostObject();
	
	detector->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + character_offset, transform->GetGlobalRotation()));
	detector->setCollisionFlags(detector->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	detector->setCollisionShape(shape);
	App->physics->AddDetector(detector);

	collider = new ComponentCollider(game_object_attached);
	collider->internal_collider = true;
	collider->detector = detector;
	
	detector->setUserPointer(collider);

}

ComponentCharacterController::~ComponentCharacterController()
{
	App->physics->RemoveAction(controller);
	App->physics->RemoveDetector(body);
	App->physics->RemoveDetector(detector);

	delete shape;
	delete body;
	delete detector;
	delete collider;
}

// Movement Functions -----------------------------------------

void ComponentCharacterController::SetWalkDirection(float3 direction)
{
	controller->setWalkDirection(ToBtVector3(direction));
}


void ComponentCharacterController::SetJumpSpeed(const float jump_speed)
{
	this->jump_speed = jump_speed;
	controller->setJumpSpeed(jump_speed);
}

void ComponentCharacterController::SetGravity(const float gravity)
{
	this->gravity = gravity;
	controller->setGravity(ToBtVector3(float3(0.f, -gravity, 0.f)));
}

void ComponentCharacterController::ApplyImpulse(float3 direction)
{
	controller->applyImpulse({ direction.x, direction.y, direction.z });
}

void ComponentCharacterController::Jump(float3 direction)
{
	controller->jump(ToBtVector3(direction));
}

bool ComponentCharacterController::CanJump()
{
	return controller->canJump();
}

bool ComponentCharacterController::OnGround()
{
	return controller->onGround();
}

void ComponentCharacterController::SetRotation(const Quat rotation)
{
	body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + character_offset, rotation));
	transform->SetGlobalRotation(math::Quat(rotation));
}

Quat ComponentCharacterController::GetRotation() const
{
	return transform->GetGlobalRotation();
}

void ComponentCharacterController::SetPosition(const float3 pos)
{
	body->setWorldTransform(ToBtTransform(pos + character_offset, transform->GetGlobalRotation()));
	transform->SetGlobalPosition(pos);
}

float3 ComponentCharacterController::GetPosition() const
{
	return transform->GetGlobalPosition();
}

void ComponentCharacterController::SetCharacterOffset(const float3 offset)
{
	character_offset = offset;
}

void ComponentCharacterController::SetCharacterHeight(const float height)
{
	float max = FLOAT_INF, min = 0.1f;
	character_height = Clamp(height, min, max);
	RecreateCapusle();
}

void ComponentCharacterController::SetCharacterRadius(const float radius)
{
	float max = FLOAT_INF, min = 0.1f;
	character_radius = Clamp(radius, min, max);
	RecreateCapusle();
}
// -------------------------------------------------------------

void ComponentCharacterController::RecreateCapusle()
{
	shape->setLocalScaling(btVector3(character_radius * 2, character_height, character_radius * 2));
}

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);

	to_save->SetFloat3("CharacterOffset", character_offset);
	to_save->SetNumber("CharacterRadius", character_radius);
	to_save->SetNumber("CharacterHeight", character_height);
	to_save->SetNumber("JumpSpeed", jump_speed);
	to_save->SetNumber("Gravity", gravity);
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
	SetCharacterOffset(to_load->GetFloat3("CharacterOffset"));
	SetCharacterRadius(to_load->GetNumber("CharacterRadius"));
	SetCharacterHeight(to_load->GetNumber("CharacterHeight"));
	SetJumpSpeed(to_load->GetNumber("JumpSpeed"));
	SetGravity(to_load->GetNumber("Gravity"));
}

bool ComponentCharacterController::DrawInspector()
{
	float3 current_character_offset = character_offset;
	float current_character_radius = character_radius;
	float current_character_height = character_height;
	float current_jump_speed = jump_speed;
	float current_gravity = gravity;

	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Title("Offset", 1);				if (ImGui::DragFloat3("##center", current_character_offset.ptr(), 0.05f)) { SetCharacterOffset(current_character_offset); }
		ImGui::Title("Radius", 1);				if (ImGui::DragFloat("##radius", &current_character_radius, 0.05f, 0.1f, FLT_MAX)) { SetCharacterRadius(current_character_radius); }
		ImGui::Title("Height", 1);				if (ImGui::DragFloat("##height", &current_character_height, 0.05f, 0.1f, FLT_MAX)) { SetCharacterHeight(current_character_height); }
		ImGui::Title("Jump Speed", 1);			if (ImGui::DragFloat("##jump_speed", &current_jump_speed, 0.01f, 0.00f, FLT_MAX)) { SetJumpSpeed(current_jump_speed); }
		ImGui::Title("Gravity", 1);				if (ImGui::DragFloat("##gravity", &current_gravity, 0.01f, 0.00f, FLT_MAX)) { SetGravity(current_gravity); }
		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentCharacterController::Update()
{

	collider->Update();

	if (Time::IsPlaying())
	{
		float3 movement = float3::zero();

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT)
			movement.x -= 1;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT)
			movement.x += 1;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT)
			movement.z += 1;
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT)
			movement.z -= 1;

		float speed = (movement.Equals(float3::zero())) ? 0.f : 10.f;
		controller->setWalkDirection(ToBtVector3(movement.Normalized() * speed * Time::GetDT()));

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_STATE::KEY_REPEAT && CanJump())
			Jump();

		btTransform bt_transform = body->getWorldTransform();
		btQuaternion rotation = bt_transform.getRotation();
		btVector3 position = bt_transform.getOrigin() - ToBtVector3(character_offset);

		transform->SetGlobalPosition(float3(position));
		transform->SetGlobalRotation(math::Quat(rotation));
	}
	else
	{
		body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + character_offset, transform->GetGlobalRotation()));
	}
}

void ComponentCharacterController::DrawScene()
{
	if (game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		App->physics->DrawCharacterController(this);
	}
}



void ComponentCharacterController::HandleAlienEvent(const AlienEvent& e)
{
	collider->HandleAlienEvent(e);
}


void ComponentCharacterController::Reset()
{

}



