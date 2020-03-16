#include "Application.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCharacterController.h"
#include "ComponentCollider.h"
#include "ComponentCapsuleCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentScript.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "Alien.h"

ComponentCharacterController::ComponentCharacterController(GameObject* go) : Component(go)
{
	type = ComponentType::CHARACTER_CONTROLLER;
	// GameObject Components 
	transform =  game_object_attached->GetComponent<ComponentTransform>();
	shape = new btCapsuleShapeZ(0.5f, 1);

	body = new btPairCachingGhostObject();
	body->setCollisionShape(shape);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
	body->setWorldTransform(btTransform().getIdentity());

	controller = new btKinematicCharacterController(body, (btConvexShape*)shape, 0.5);
	App->physics->world->addCollisionObject(body, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	App->physics->AddAction(controller);
}

ComponentCharacterController::~ComponentCharacterController()
{
	App->physics->RemoveAction(controller);
	App->physics->world->removeCollisionObject(body);

	delete shape;
}

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
}

void ComponentCharacterController::Update()
{
	controller->setGravity(btVector3(0.f,-10.f, 0.f));
}

void ComponentCharacterController::DrawScene()
{
	App->physics->DrawCharacterController(this);


}

void ComponentCharacterController::Reset()
{
}

bool ComponentCharacterController::DrawInspector()
{
	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
	}


	return true;
}

