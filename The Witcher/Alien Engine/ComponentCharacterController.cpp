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

	// Create Callback

	desc.material = material;
	desc.reportCallback = report = new UserControllerHitReport();
	report->controller = this;

	SetDefaultConf();
	CreateController();
	physics->AddController(this);
}

ComponentCharacterController::~ComponentCharacterController()
{
	go->SendAlientEventThis(this, AlienEventType::CHARACTER_CTRL_DELETED);

	DeleteController();

	if (report) {
		delete report;
	}
}

bool ComponentCharacterController::CharacterEnabled()
{
	return controller ;
}

void ComponentCharacterController::CreateController()
{
	if (enabled && go->IsEnabled() && !controller) {
		desc.position = F3_TO_PXVEC3EXT(game_object_attached->transform->GetGlobalPosition() - controller_offset);
		controller = App->physx->CreateCharacterController(desc);
		LinkShapesToComponent();
		SetCollisionLayer(layer_name);
		UpdateParameters();
	}
}

void ComponentCharacterController::DeleteController()
{
	if (controller != nullptr) {
		PX_RELEASE(controller);
		controller = nullptr;
	}
}

void ComponentCharacterController::OnEnable()
{
	CreateController();
	physics->PutToSleep();
}

void ComponentCharacterController::OnDisable()
{
	DeleteController();
	physics->WakeUp();
}

void ComponentCharacterController::SetDefaultConf()
{
	move_direction = controller_offset = float3::zero();
	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	desc.radius = 0.5f;
	desc.height = 1.0f;
	min_distance = 0.001f;
	desc.slopeLimit = cosf(DegToRad(45.0f));
}

void ComponentCharacterController::LinkShapesToComponent()
{
	PxShape* shapes[10];
	Uint32 nb_shapes = controller->getActor()->getNbShapes();
	controller->getActor()->getShapes(shapes, nb_shapes);

	for (uint i = 0; i < nb_shapes; ++i)
		shapes[i]->userData = this;

	controller->setUserData(this);
}


// Movement Functions -----------------------------------------

void ComponentCharacterController::SetContactOffset(const float contactOffset)
{
	float max = FLT_MAX, min = 0.0001f;
	desc.contactOffset = Clamp(contactOffset, min, max);

	if (!CharacterEnabled()) return;

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

	if (!CharacterEnabled()) return;

	switch (desc.getType())
	{
	case PxControllerShapeType::eCAPSULE: {
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

	if (!CharacterEnabled()) return;

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

	if (!CharacterEnabled()) return;

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

	if (!CharacterEnabled()) return;

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

	if (!CharacterEnabled()) return;

	controller->setPosition(F3_TO_PXVEC3EXT(transform->GetGlobalPosition() + controller_offset));
}
// -------------------------------------------------------------

void ComponentCharacterController::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
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

	enabled = to_load->GetBoolean("Enabled");

	if (!enabled)
	{
		OnDisable();
	}

	gravity = to_load->GetNumber("Gravity");
	force_gravity = to_load->GetBoolean("ForceGravity");
	force_move = to_load->GetBoolean("ForceMove");
	min_distance = to_load->GetNumber("MinMoveDistance");
	
	SetSlopeLimit(to_load->GetNumber("SlopeLimit"));
	SetContactOffset(to_load->GetNumber("SkinWidth"));
	SetCharacterOffset(to_load->GetFloat3("Center"));
	SetCharacterRadius(to_load->GetNumber("CharacterRadius"));
	SetCharacterHeight(to_load->GetNumber("CharacterHeight"));
	SetCollisionLayer(to_load->GetString("CollisionLayer", "Default"));

	SetStepOffset(to_load->GetNumber("StepOffset"));
	SetPosition(transform->GetGlobalPosition());
}

void ComponentCharacterController::UpdateParameters()
{
	SetSlopeLimit(desc.slopeLimit);
	SetStepOffset(desc.stepOffset);
	SetContactOffset(desc.contactOffset);
	SetCharacterOffset(controller_offset);
	SetCharacterRadius(desc.radius);
	SetCharacterHeight(desc.height);
	SetCollisionLayer(layer_name);
}

bool ComponentCharacterController::DrawInspector()
{
	static bool check;
	check = enabled;

	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &check)) {
		enabled = check;
		if (!enabled) {
			OnDisable();
		}
		else {
			OnEnable();

		}
	}

	ImGui::SameLine();

	if (ImGui::CollapsingHeader(" Character Controller", &not_destroy))
	{
		ImGui::Spacing();

		DrawLayersCombo();
		float slopeLimitDeg = RadToDeg(acosf((float)desc.slopeLimit));
		ImGui::Title("Slope Limit", 1);				if (ImGui::DragFloat("##slopeLimit", &slopeLimitDeg, 0.03f, 0.0f, 180.0f)) { SetSlopeLimit(slopeLimitDeg); }
		ImGui::Title("Step Offset", 1);				if (ImGui::DragFloat("##stepOffset", &desc.stepOffset, 0.03f, 0.0f, FLT_MAX)) { SetStepOffset(desc.stepOffset); }
		ImGui::Title("Skin Width", 1);				if (ImGui::DragFloat("##skinWidth", &desc.contactOffset, 0.03f, 0.0001f, FLT_MAX)) { SetContactOffset(desc.contactOffset); }
		ImGui::Title("Min Move Distance", 1);		ImGui::DragFloat("##minDist", &min_distance, 0.03f, 0.f, FLT_MAX);
		ImGui::Title("Center", 1);					if (ImGui::DragFloat3("##center", controller_offset.ptr(), 0.03f, -math::inf, math::inf)) { SetCharacterOffset(controller_offset); }
		ImGui::Title("Radius", 1);					if (ImGui::DragFloat("##radius", &desc.radius, 0.05f, 0.1f, FLT_MAX)) { SetCharacterRadius(desc.radius); }
		ImGui::Title("Height", 1);					if (ImGui::DragFloat("##height", &desc.height, 0.05f, 0.0f, FLT_MAX)) { SetCharacterHeight(desc.height); }

		ImGui::Separator();
		ImGui::Title("Experimental features", 0);
		ImGui::Separator();
		ImGui::Title("Gravity", 1);					ImGui::DragFloat("##gravity", &gravity, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Force gravity", 1);			ImGui::Checkbox("##forceGravity", &force_gravity);
		ImGui::Title("Force move", 1);				ImGui::Checkbox("##forceMove", &force_move);

		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentCharacterController::Update()
{
	if (!CharacterEnabled()) return;

	if (!Time::IsPlaying() || physics->gizmo_selected) {
		controller->setPosition(F3_TO_PXVEC3EXT(transform->GetGlobalPosition() + controller_offset));
	}
	else {
		if (force_move && isGrounded && velocity.isZero()) {
			Move(float3(0.0f, -controller->getContactOffset(), 0.0f));
		}
		if (force_gravity && !isGrounded) {
			move_direction.y -= gravity * Time::GetDT();
			Move(move_direction * Time::GetDT());
			isGrounded ? move_direction.y = 0.0f : 0;
		}
		transform->SetGlobalPosition(PXVEC3EXT_TO_F3(controller->getPosition()) - controller_offset);
	}
}

// always force downwards to mantain collision state 
// (cct solver puts on top of skin and not returns collision state when performs a 0y)
PxControllerCollisionFlags ComponentCharacterController::Move(float3 motion)
{
	if (!enabled) return collisionFlags;

	// Filter data -------------------------
	PxFilterData filter_data(layer_num, physics->ID, 0, 0);
	PxControllerFilters filters(&filter_data, App->physx->px_controller_filter); 
	PxExtendedVec3 last_position = controller->getPosition();

	collisionFlags = controller->move(F3_TO_PXVEC3(motion), min_distance, Time::GetDT(), filters);
	collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN) ? isGrounded = true : isGrounded = false; 	// set grounded internal state
	velocity = isGrounded ? F3_TO_PXVEC3EXT(float3::zero()) : PXVEC3_TO_VEC3EXT(controller->getPosition() - last_position); 	// substract the difference from current pos to velocity before move

	return collisionFlags;
}

void ComponentCharacterController::SetCollisionLayer(std::string layer)
{
	int index = 0;
	if (!physics->layers->GetIndexByName(layer, index)) return;
	layer_num = index;
	layer_name = layer;

	if (!CharacterEnabled()) return;

	PxShape* shapes[10];
	PxRigidDynamic* actor = controller->getActor();
	Uint32 ns = actor->getNbShapes();
	controller->getActor()->getShapes(shapes, ns);
	PxFilterData filter_data(layer_num, physics->ID, 0, 0);

	for (uint i = 0; i < ns; ++i) {
		shapes[i]->setSimulationFilterData(filter_data);
		shapes[i]->setQueryFilterData(filter_data);
	}
}

void ComponentCharacterController::DrawScene()
{
	if (!CharacterEnabled()) return;

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

void ComponentCharacterController::OnControllerColliderHit(ControllerColliderHit hit)
{
	Alien* alien = nullptr;

	if (!CharacterEnabled()) return;

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
	if (controller != nullptr) {
		return controller->setPosition(F3_TO_PXVEC3EXT(position + controller_offset));
	}
	return false;
}

bool ComponentCharacterController::SetFootPosition(float3 position) const
{
	return controller->setFootPosition(F3_TO_PXVEC3EXT(position));
}

float3 ComponentCharacterController::GetPosition() const
{
	return  PXVEC3EXT_TO_F3(controller->getPosition());
}

float3 ComponentCharacterController::GetFootPosition() const
{
	return  PXVEC3EXT_TO_F3(controller->getFootPosition());
}

float ComponentCharacterController::GetSlopeLimit() const
{
	return RadToDeg(acosf((float)desc.slopeLimit));
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
		_hit.rigidbody = col->physics->GetRigidBody();
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
	_hit.rigidbody = _hit.collider->physics->GetRigidBody();
	_hit.transform = _hit.gameObject->transform;

	_hit.moveDirection = PXVEC3_TO_F3(hit.dir);
	_hit.moveLength = hit.length;
	_hit.normal = PXVEC3_TO_F3(hit.worldNormal);
	_hit.point = PXVEC3EXT_TO_F3(hit.worldPos);

	controller->OnControllerColliderHit(_hit);
}

void UserControllerHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{

}