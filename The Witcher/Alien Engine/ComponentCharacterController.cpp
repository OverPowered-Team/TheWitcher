#include "Application.h"
#include "ModulePhysX.h"
#include "ModuleRenderer3D.h"
#include "ComponentCharacterController.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ReturnZ.h"

#include "UtilitiesPhysX.h"
#include "ComponentPhysics.h"
#include "ComponentScript.h"
#include "Alien.h"

#include "Time.h"
#include "ModuleInput.h"
#include "mmgr/mmgr.h"

ComponentCharacterController::ComponentCharacterController(GameObject* go) : ComponentCollider(go)
{
	type = ComponentType::CHARACTER_CONTROLLER;
	// GameObject Components 
	transform = go->GetComponent<ComponentTransform>();

	// load default settings
	SetDefaultConf();
	controller = App->physx->CreateCharacterController(desc);

	moveDirection = controller_offset = float3::zero();

	// links hidden kinematic actor shapes with user data
	// contacts callback method currently needs user data on shape ptr
	Uint32 ns = controller->getActor()->getNbShapes();
	PxShape* all_shapes;
	controller->getActor()->getShapes(&all_shapes, ns);
	for (uint i = 0; i < ns; ++i)
		all_shapes[i].userData = this;

	controller->setUserData(this);
	go->SendAlientEventThis(this, AlienEventType::CHARACTER_CTRL_ADDED);
	SetCollisionLayer("Default");
}

ComponentCharacterController::~ComponentCharacterController()
{
	go->SendAlientEventThis(this, AlienEventType::CHARACTER_CTRL_DELETED);
	controller->release();
	delete report;
}

// Movement Functions -----------------------------------------

void ComponentCharacterController::SetContactOffset(const float contactOffset)
{
	float max = FLT_MAX, min = 0.0001f;
	desc.contactOffset = Clamp(contactOffset, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setContactOffset(desc.contactOffset);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetStepOffset(const float stepOffset)
{
	float max = FLT_MAX, min = 0.0f;
	desc.stepOffset = Clamp(stepOffset, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setStepOffset(desc.stepOffset);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetSlopeLimit(const float slopeLimit)
{
	float max = 180.0f, min = 0.0f;
	float _slopeLimit = Clamp(slopeLimit, min, max);
	desc.slopeLimit = cosf(DegToRad(_slopeLimit));

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setSlopeLimit(desc.slopeLimit);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetCharacterHeight(const float height)
{
	float max = FLOAT_INF, min = 0.0f; // 0 means sphere
	desc.height = Clamp(height, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setHeight(desc.height);
		break;
	}
	default:
		break;
	}
}

void ComponentCharacterController::SetCharacterRadius(const float radius)
{
	float max = FLOAT_INF, min = 0.1f;
	desc.radius = Clamp(radius, min, max);

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE:
	{
		PxCapsuleController* capsule_c = (PxCapsuleController*)controller;
		capsule_c->setRadius(desc.radius);
		break;
	}
	default:
		break;
	}

}

void ComponentCharacterController::SetCharacterOffset(float3 offset)
{
	controller_offset = offset;
	controller->setPosition(F3_TO_PXVEC3EXT(transform->GetGlobalPosition() + controller_offset));
}
// -------------------------------------------------------------

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);

	to_save->SetNumber("SlopeLimit", RadToDeg(acosf(desc.slopeLimit)));
	to_save->SetNumber("StepOffset", desc.stepOffset);
	to_save->SetNumber("SkinWidth", desc.contactOffset);
	to_save->SetNumber("MinMoveDistance", min_distance);
	to_save->SetFloat3("Center", controller_offset);
	to_save->SetNumber("CharacterRadius", desc.radius);
	to_save->SetNumber("CharacterHeight", desc.height);

	to_save->SetNumber("Gravity", gravity);
	to_save->SetBoolean("ForceGravity", force_gravity);
	to_save->SetBoolean("ForceMove", force_move);

	to_save->SetString("CollisionLayer", layer_name.c_str());
}

void ComponentCharacterController::LoadComponent(JSONArraypack* to_load)
{
	SetSlopeLimit(to_load->GetNumber("SlopeLimit"));
	SetStepOffset(to_load->GetNumber("StepOffset"));
	SetContactOffset(to_load->GetNumber("SkinWidth"));
	min_distance = to_load->GetNumber("MinMoveDistance");
	SetCharacterOffset(to_load->GetFloat3("Center"));
	SetCharacterRadius(to_load->GetNumber("CharacterRadius"));
	SetCharacterHeight(to_load->GetNumber("CharacterHeight"));

	gravity = to_load->GetNumber("Gravity");
	force_gravity = to_load->GetBoolean("ForceGravity");
	force_move = to_load->GetBoolean("ForceMove");

	SetCollisionLayer(to_load->GetString("CollisionLayer", "Default"));
}

bool ComponentCharacterController::DrawInspector()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy))
	{
		ImGui::Spacing();
	
		DrawLayersCombo();
		float slopeLimitDeg = RadToDeg(acosf((float)desc.slopeLimit));
		ImGui::Title("Slope Limit", 1);				if (ImGui::DragFloat("##slopeLimit", &slopeLimitDeg, 0.03f, 0.0f, 180.0f))					{ SetSlopeLimit(slopeLimitDeg); }
		ImGui::Title("Step Offset", 1);				if (ImGui::DragFloat("##stepOffset", &desc.stepOffset, 0.03f, 0.0f, FLT_MAX))				{ SetStepOffset(desc.stepOffset); }
		ImGui::Title("Skin Width", 1);				if (ImGui::DragFloat("##skinWidth", &desc.contactOffset, 0.03f, 0.0001f, FLT_MAX))			{ SetContactOffset(desc.contactOffset); }
		ImGui::Title("Min Move Distance", 1);		ImGui::DragFloat("##minDist", &min_distance, 0.03f, 0.f, FLT_MAX);
		ImGui::Title("Center", 1);					if (ImGui::DragFloat3("##center", controller_offset.ptr(), 0.03f, -math::inf, math::inf))	{ SetCharacterOffset(controller_offset); }
		ImGui::Title("Radius", 1);					if (ImGui::DragFloat("##radius", &desc.radius, 0.05f, 0.1f, FLT_MAX))						{ SetCharacterRadius(desc.radius); }
		ImGui::Title("Height", 1);					if (ImGui::DragFloat("##height", &desc.height, 0.05f, 0.0f, FLT_MAX))						{ SetCharacterHeight(desc.height); }
		
		ImGui::Separator();
		ImGui::Title("Experimental features",0);
		ImGui::Separator();
		ImGui::Title("Gravity", 1);					ImGui::DragFloat("##gravity", &gravity, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Force gravity", 1);			ImGui::Checkbox("##forceGravity", &force_gravity);
		ImGui::Title("Force move", 1);				ImGui::Checkbox("##forceMove", &force_move);
		
		
		//ImGui::Checkbox("isGrounded", &isGrounded); // debug test

		/*ImGui::Separator();
		ImGui::Text("velocity: %f,%f,%f", velocity.x, velocity.y, velocity.z);*/
		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentCharacterController::Update()
{
	//collider->Update();

	if (Time::IsPlaying())
	{
		// TODO DELETE hardcoded test 1 -----------------------------------------------------------------------
		//moveDirection.x = moveDirection.z = 0.0f;
		//float speed = 10.0f;
		//if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.x -= 1.0f;
		//if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.x += 1;
		//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.z += 1;
		//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT)
		//	moveDirection.z -= 1;
		//if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_STATE::KEY_DOWN)
		//	moveDirection.y = 8.0f;

		//moveDirection = float3(moveDirection.x * speed, moveDirection.y, moveDirection.z * speed) ; // SPEED
		// ----------------------------------------------------------------------------------------------------

		if (force_move && isGrounded && velocity.isZero())
		{
			// always force downwards to mantain collision state 
			// (cct solver puts on top of skin and not returns collision state when performs a 0y)
			Move(float3(0.0f, -controller->getContactOffset(), 0.0f));
		}

		if (force_gravity && !isGrounded)
		{
			moveDirection.y -= gravity * Time::GetDT();
			Move(moveDirection * Time::GetDT());
			isGrounded ? moveDirection.y = 0.0f : 0;
		}
		//else // TODO: DELETE HARDCODED TEST from 1
		//{
		//	Move(float3(moveDirection.x, moveDirection.y - controller->getContactOffset(), moveDirection.z) * Time::GetDT());
		//}

		transform->SetGlobalPosition(PXVEC3EXT_TO_F3(controller->getPosition()) - controller_offset);
	}
	else
	{
		controller->setPosition(F3_TO_PXVEC3EXT(transform->GetGlobalPosition() + controller_offset));
	}
}

PxControllerCollisionFlags ComponentCharacterController::Move(float3 motion)
{

	// set velocity on current position before move
	velocity = controller->getPosition();

	// perform the move
	PxFilterData filter_data( layer_num, physics->ID, 0, 0);
	PxControllerFilters filters(&filter_data, App->physx->px_controller_filter_callback); // TODO: implement filters callback when needed
	collisionFlags = controller->move(F3_TO_PXVEC3(motion), min_distance, Time::GetDT(), filters);

	// set grounded internal state
	collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN) ? isGrounded = true : isGrounded = false;
	// substract the difference from current pos to velocity before move
	velocity = isGrounded ? F3_TO_PXVEC3EXT(float3::zero()) : PXVEC3_TO_VEC3EXT(controller->getPosition() - velocity);

	return collisionFlags;
}

void ComponentCharacterController::SetCollisionLayer(std::string layer)
{
	int index = 0;
	if (!physics->layers->GetIndexByName(layer, index)) return;
	layer_num = index;
	layer_name = layer;

	PxShape* all_shapes;
	Uint32 ns = controller->getActor()->getNbShapes();
	controller->getActor()->getShapes(&all_shapes, ns);
	PxFilterData filter_data(layer_num, physics->ID, 0, 0);

	for (uint i = 0; i < ns; ++i) {
		all_shapes[i].setSimulationFilterData(filter_data);
		all_shapes[i].setQueryFilterData(filter_data);
	}
}

void ComponentCharacterController::DrawScene(ComponentCamera* camera)
{
	if (game_object_attached->IsSelected() && App->physx->debug_physics == false)
	{
		switch (desc.getType())
		{
		case PxControllerShapeType::eCAPSULE:
		{
			PxCapsuleController* capsule = (PxCapsuleController*)controller;
			float skin_offset = capsule->getContactOffset();

			//float4x4 transform = PXTRANS_TO_F4X4(capsule->getActor()->getGlobalPose());

			float radius = capsule->getRadius() + skin_offset;
			float height = capsule->getHeight() + skin_offset; //


			float3 oriblue(0.15f, 0.3f, 0.95f);
			PxExtendedVec3 pos = capsule->getPosition();
			PxTransform trans;
			trans.p = PxVec3(pos.x, pos.y, pos.z);
			trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(0.0f, 0.0f, DEGTORAD * 90.0f));
			App->renderer3D->DebugDrawCapsule(PXTRANS_TO_F4X4(trans), radius, height * 0.5f, oriblue);

			break;
		}
		case PxControllerShapeType::eBOX:
		{
			break;
		}
		default:
			break;
		}
	}

}


void ComponentCharacterController::HandleAlienEvent(const AlienEvent& e)
{
}

void ComponentCharacterController::SetDefaultConf()
{
	desc.radius = 0.5f;
	desc.height = 2.0f;
	if (desc.material) desc.material->release();
	desc.material = App->physx->CreateMaterial(static_friction, dynamic_friction, restitution);
	desc.position = F3_TO_PXVEC3EXT(game_object_attached->transform->GetGlobalPosition());
	desc.slopeLimit = cosf(DegToRad(45.0f));
	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	// hit report callback
	if (report) delete report;
	desc.reportCallback = report = new UserControllerHitReport();
	report->controller = this;
	// 

	//desc.invisibleWallHeight = 100.0f;// TODO: implement if needed automatic invisible walls
	//desc.maxJumpHeight = 1.0f;
	//desc.upDirection = // up direction can be changed to simulate planetoids or surfaces with other gravity dir vector

	min_distance = 0.001f;
}

void ComponentCharacterController::OnControllerColliderHit(ControllerColliderHit hit)
{
	Alien* alien = nullptr;
	for (ComponentScript* script : physics->scripts)
	{
		try {
			alien = (Alien*)script->data_ptr;
			alien->OnControllerColliderHit(hit);
		}
		catch (...) {
			LOG_ENGINE("Error on script \"%s\" when calling \"%s\"", alien->data_name, hit.gameObject->GetName());
		}
	}
}

bool ComponentCharacterController::SetPosition(float3 position) const
{
	return controller->setPosition(F3_TO_PXVEC3EXT(position));
}

float3 ComponentCharacterController::GetPosition() const
{
	return  PXVEC3EXT_TO_F3(controller->getPosition());
}

float3 ComponentCharacterController::GetFootPosition() const
{
	return  PXVEC3EXT_TO_F3(controller->getFootPosition());
}

//* -------------------------- User hit callbacks
void UserControllerHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
	ControllerColliderHit _hit;
	
	ComponentCollider* col = (ComponentCollider*)hit.shape->userData;
	_hit.collider = col;
	if (_hit.collider)
	{
		//_hit.controller = (ComponentCharacterController*)col; // only onControllerHit
		_hit.gameObject = (GameObject*)col->game_object_attached;
		_hit.rigidbody = col->physics->rigid_body;
		_hit.transform = _hit.gameObject->transform;
	}

	_hit.moveDirection = PXVEC3_TO_F3(hit.dir);
	_hit.moveLength = hit.length;
	_hit.normal = PXVEC3_TO_F3(hit.worldNormal);
	_hit.point = PXVEC3EXT_TO_F3(hit.worldPos);
	
	controller->OnControllerColliderHit(_hit);
}

void UserControllerHitReport::onControllerHit(const PxControllersHit& hit)
{
	ControllerColliderHit _hit;

	_hit.controller = (ComponentCharacterController*)hit.other->getUserData();
	_hit.collider = (ComponentCollider*)_hit.controller;
	_hit.gameObject = _hit.controller->game_object_attached;
	_hit.rigidbody = _hit.collider->physics->rigid_body;
	_hit.transform = _hit.gameObject->transform;
	
	_hit.moveDirection = PXVEC3_TO_F3(hit.dir);
	_hit.moveLength = hit.length;
	_hit.normal = PXVEC3_TO_F3(hit.worldNormal);
	_hit.point = PXVEC3EXT_TO_F3(hit.worldPos);

	controller->OnControllerColliderHit(_hit);
}

void UserControllerHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
	LOG_ENGINE("OBSTACLE HIT");
}