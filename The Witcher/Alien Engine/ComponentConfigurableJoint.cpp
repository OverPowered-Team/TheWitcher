#include "Application.h"
#include "ComponentConfigurableJoint.h"
#include "ComponentRigidBody.h"
#include "ComponentPhysics.h"
#include "GameObject.h"
#include "ModuleObjects.h"
#include "ModulePhysX.h"

ComponentConfigurableJoint::ComponentConfigurableJoint(GameObject* go) : ComponentJoint(go)
{
	// GameObject Components 
	type = ComponentType::CONFIGURABLE_JOINT;
	name = "Configurable Joint";

	ComponentRigidBody* rb = go->GetComponent<ComponentRigidBody>();
	if (rb) CreateJoint();
}

PxJoint* ComponentConfigurableJoint::CreateSpecificJoint()
{
	PxTransform trans(PxVec3(0, 0, 0));
	return physx::PxD6JointCreate(*App->physx->px_physics, physics->actor,trans, nullptr, trans);
}

void ComponentConfigurableJoint::SetupSpecificJoint()
{
	PxD6Joint* spe_joint = (PxD6Joint*) joint;
	
}

void ComponentConfigurableJoint::DrawInspectorJoint()
{

}

void ComponentConfigurableJoint::SaveComponentJoint(JSONArraypack* to_save)
{
}

void ComponentConfigurableJoint::LoadComponentJoint(JSONArraypack* to_load)
{
}
