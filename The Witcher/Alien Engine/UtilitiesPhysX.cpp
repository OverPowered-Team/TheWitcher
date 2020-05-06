#include "Application.h"
#include "ModulePhysX.h"
#include "UtilitiesPhysX.h"
#include "ComponentScript.h"
#include "ComponentPhysics.h"
#include "ComponentRigidBody.h"
#include "GameObject.h"
#include "Alien.h"

void CustomErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	switch (code)
	{
	case physx::PxErrorCode::Enum::eNO_ERROR:
		LOG_ENGINE("eNO_ERROR: %s", message);
		break;
	case physx::PxErrorCode::Enum::eDEBUG_INFO: //! \brief An informational message
		LOG_ENGINE("eDEBUG_INFO: %s", message);
		break;
	case physx::PxErrorCode::Enum::eDEBUG_WARNING: //! \brief a warning message for the user to help with debugging
		LOG_ENGINE("eDEBUG_WARNING: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINVALID_PARAMETER: //! \brief method called with invalid parameter(s)
		LOG_ENGINE("eINVALID_PARAMETER: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINVALID_OPERATION: //! \brief method was called at a time when an operation is not possible
		LOG_ENGINE("eINVALID_OPERATION: %s", message);
		break;
	case physx::PxErrorCode::Enum::eOUT_OF_MEMORY: //! \brief method failed to allocate some memory
		LOG_ENGINE("eOUT_OF_MEMORY: %s", message);
		break;
	case physx::PxErrorCode::Enum::eINTERNAL_ERROR: //! \brief The library failed for some reason. Possibly you have passed invalid values like NaNs, which are not checked for
		LOG_ENGINE("eINTERNAL_ERROR: %s", message);
		break;
	case physx::PxErrorCode::Enum::eABORT: //! \brief An unrecoverable error, execution should be halted and log output flushed
		LOG_ENGINE("eABORT: %s", message);
		break;
	case physx::PxErrorCode::Enum::ePERF_WARNING: //! \brief The SDK has determined that an operation may result in poor performance
		LOG_ENGINE("ePERF_WARNING: %s", message);
		break;
	case physx::PxErrorCode::Enum::eMASK_ALL: //! \brief A bit mask for including all errors
		LOG_ENGINE("eMASK_ALL: %s", message);
		break;
	}
}


void SimulationEventCallback::onContact(const PxContactPairHeader& pair_header, const PxContactPair* pairs, PxU32 num_pairs)
{
	ComponentCollider* collider_A = nullptr;
	ComponentCollider* collider_B = nullptr;
	ComponentRigidBody* rb_A = nullptr;
	ComponentRigidBody* rb_B = nullptr;
	std::vector<ContactPoint> contact_points_A;
	std::vector<ContactPoint> contact_points_B;
	float3 point = float3(0.f, 0.f, 0.f);
	float3 normal = float3(0.f, 0.f, 0.f);
	float3 point_impulse = float3(0.f, 0.f, 0.f);
	float3 impulse = float3(0.f, 0.f, 0.f);
	float3 relative_velocity_A = float3(0.f, 0.f, 0.f);
	float3 relative_velocity_B = float3(0.f, 0.f, 0.f);
	Alien* alien = nullptr;
	CallbackType callback_type = CallbackType::UNKNOWN;

	for (physx::PxU32 i = 0; i < num_pairs; ++i)
	{
		const physx::PxContactPair& contact_pair = pairs[i];
		collider_A = (ComponentCollider*)contact_pair.shapes[0]->userData;
		collider_B = (ComponentCollider*)contact_pair.shapes[1]->userData;
		if (!collider_A || !collider_B) continue;
		if (contact_pair.flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1)) continue;
		if (collider_A->physics->scripts.empty() && collider_B->physics->scripts.empty()) continue;  // If any collider has scripts discard
		if (contact_pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)	      callback_type = CallbackType::ENTER;
		else if (contact_pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS) callback_type = CallbackType::STAY;
		else if (contact_pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)     callback_type = CallbackType::EXIT;

		rb_A = collider_B->physics->GetRigidBody();
		rb_B = collider_A->physics->GetRigidBody();

		if (contact_pair.contactCount > 0)
		{
			std::vector<physx::PxContactPairPoint> contact_points;
			contact_points.resize(contact_pair.contactCount);
			contact_pair.extractContacts(&contact_points[0], contact_pair.contactCount);

			for (PxContactPairPoint contact_point : contact_points)
			{
				point = PXVEC3_TO_F3(contact_point.position);
				normal = PXVEC3_TO_F3(contact_point.normal);
				point_impulse = PXVEC3_TO_F3(contact_point.impulse);
				impulse += point_impulse;
				contact_points_A.push_back(ContactPoint(normal, point, contact_point.separation, collider_A, collider_B));
				contact_points_B.push_back(ContactPoint(normal, point, contact_point.separation, collider_B, collider_A));
			}
		}

		relative_velocity_A = ((rb_A) ? rb_A->GetVelocity() : float3::zero()) - ((rb_B) ? rb_B->GetVelocity() : float3::zero());
		relative_velocity_B = ((rb_B) ? rb_B->GetVelocity() : float3::zero()) - ((rb_A) ? rb_A->GetVelocity() : float3::zero());


		Collision collision_A(collider_B, rb_A, collider_B->transform, contact_points_A, contact_pair.contactCount, collider_B->game_object_attached, impulse, relative_velocity_A);
		Collision collision_B(collider_A, rb_B, collider_A->transform, contact_points_B, contact_pair.contactCount, collider_A->game_object_attached, impulse, relative_velocity_B);

		for (ComponentScript* script : collider_A->physics->scripts)
		{
			try {
				alien = (Alien*)script->data_ptr;
				if (callback_type == CallbackType::ENTER)		alien->OnCollisionEnter(collision_A);
				else if (callback_type == CallbackType::STAY)	alien->OnCollisionStay(collision_A);
				else if (callback_type == CallbackType::EXIT)	alien->OnCollisionExit(collision_A);
			}
			catch (...) {
				LOG_ENGINE("Error on script \"%s\" when calling \"%s\"", alien->data_name, oncollision_names[(int)callback_type]);
			}
		}

		for (ComponentScript* script : collider_B->physics->scripts)
		{
			try {
				alien = (Alien*)script->data_ptr;
				if (callback_type == CallbackType::ENTER)		alien->OnCollisionEnter(collision_B);
				else if (callback_type == CallbackType::STAY)	alien->OnCollisionStay(collision_B);
				else if (callback_type == CallbackType::EXIT)	alien->OnCollisionExit(collision_B);
			}
			catch (...) {
				LOG_ENGINE("Error on script \"%s\" when calling \"%s\"", alien->data_name, oncollision_names[(int)callback_type]);
			}
		}

		impulse = float3(0.f, 0.f, 0.f);
		contact_points_A.clear();
		contact_points_B.clear();
		relative_velocity_A = float3(0.f, 0.f, 0.f);
		relative_velocity_B = float3(0.f, 0.f, 0.f);
	}
}

void SimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 num_pairs)
{
	Alien* alien = nullptr;
	ComponentCollider* collider_A = nullptr;
	ComponentCollider* collider_B = nullptr;
	CallbackType callback_type = CallbackType::UNKNOWN;

	for (physx::PxU32 i = 0; i < num_pairs; ++i)
	{
		const physx::PxTriggerPair& trigger_pair = pairs[i];

		collider_A = (ComponentCollider*)trigger_pair.triggerShape->userData;
		collider_B = (ComponentCollider*)trigger_pair.otherShape->userData;
		if (!collider_A || !collider_B) continue;
		if (trigger_pair.flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) continue;
		if (collider_A->physics->scripts.empty() && collider_B->physics->scripts.empty()) continue;  // If any collider has scripts discard
		if (trigger_pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)	      callback_type = CallbackType::ENTER;
		else if (trigger_pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS) callback_type = CallbackType::STAY;
		else if (trigger_pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)     callback_type = CallbackType::EXIT;

		for (ComponentScript* script : collider_A->physics->scripts)
		{
			try {
				alien = (Alien*)script->data_ptr;
				if (callback_type == CallbackType::ENTER)		alien->OnTriggerEnter(collider_B);
				else if (callback_type == CallbackType::STAY)	alien->OnTriggerStay(collider_B);
				else if (callback_type == CallbackType::EXIT)	alien->OnTriggerExit(collider_B);
			}
			catch (...) {
				LOG_ENGINE("Error on script \"%s\" when calling \"%s\"", alien->data_name, ontrigger_names[(int)callback_type]);
			}
		}

		for (ComponentScript* script : collider_B->physics->scripts)
		{
			try {
				alien = (Alien*)script->data_ptr;
				if (callback_type == CallbackType::ENTER)		alien->OnTriggerEnter(collider_A);
				else if (callback_type == CallbackType::STAY)	alien->OnTriggerStay(collider_A);
				else if (callback_type == CallbackType::EXIT)	alien->OnTriggerExit(collider_A);
			}
			catch (...) {
				LOG_ENGINE("Error on script \"%s\" when calling \"%s\"", alien->data_name, ontrigger_names[(int)callback_type]);
			}
		}
	}
}

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (!App->physx->layers.data[filterData0.word0][ filterData1.word0] ||  // Check Collision Layers a-b
		!App->physx->layers.data[filterData1.word0][filterData0.word0]  ||  // Check Collision Layers b-a
		filterData0.word1 == filterData1.word1)                             // Check Game Object ID
		return physx::PxFilterFlag::eSUPPRESS;

	if (attributes0 & PxFilterObjectFlag::eTRIGGER || attributes1 & PxFilterObjectFlag::eTRIGGER)
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
	else
	{
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	return physx::PxFilterFlags();
}

PxQueryHitType::Enum ControllerFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	PxFilterData filterData0 = filterData;
	PxFilterData filterData1 = shape->getSimulationFilterData();

	if (!App->physx->layers.data[filterData0.word0][filterData1.word0] ||  // Check Collision Layers a-b
		!App->physx->layers.data[filterData1.word0][filterData0.word0] ||  // Check Collision Layers b-a
		filterData0.word1 == filterData1.word1)                             // Check Game Object ID
		return PxQueryHitType::Enum::eNONE;
	else
		return  PxQueryHitType::Enum::eTOUCH;
}

PxQueryHitType::Enum ControllerFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	return PxQueryHitType::Enum::eTOUCH;
}

LayerChangedData::LayerChangedData(int layer_0, int layer_1) : layer_0(layer_0), layer_1(layer_1) {}

ContactPoint::ContactPoint(const float3& normal, const float3& point, float separation, ComponentCollider* this_collider, ComponentCollider* other_collider) :
	normal(normal), point(point), separation(separation), this_collider(this_collider), other_collider(other_collider) {}

Collision::Collision(ComponentCollider* collider, ComponentRigidBody* rigid_body, ComponentTransform* transform, const std::vector<ContactPoint>& contancts,
	uint num_contact, GameObject* game_object, const float3& impulse, const float3& relative_velocity) :
	collider(collider), rigid_body(rigid_body), transform(transform), contancts(contancts), num_contact(num_contact), game_object(game_object), impulse(impulse), relative_velocity(relative_velocity) {}

void RaycastHit::SetRaycastHit(const PxRaycastHit& _hit) {
	collider = (ComponentCollider*)_hit.shape->userData;
	rigid_body = collider->physics->GetRigidBody();
	transform = collider->physics->transform;
	distance = _hit.distance;
	normal = PXVEC3_TO_F3(_hit.normal);
	point = PXVEC3_TO_F3(_hit.position);
	texture_coords = float2(_hit.u, _hit.v);
}
