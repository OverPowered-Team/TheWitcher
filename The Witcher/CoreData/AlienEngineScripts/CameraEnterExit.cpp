#include "CameraEnterExit.h"
#include "PlayerController.h"
#include "TriggerCamera.h"

CameraEnterExit::CameraEnterExit() : Alien()
{
}

CameraEnterExit::~CameraEnterExit()
{
}

void CameraEnterExit::Start()
{
	script_parent = (TriggerCamera*)game_object->GetComponentScriptInParent("TriggerCamera");
}

void CameraEnterExit::OnTriggerEnter(ComponentCollider* collider)
{
	PlayerController* player = (PlayerController*)collider->game_object_attached->GetComponentScript("PlayerController");
	if (player != nullptr) {
		script_parent->RegisterMovement(player->controller_index - 1, collider_position);//INFO: Controller index starts by 1
	}
}

void CameraEnterExit::OnDrawGizmos()
{	
	if (collider_position == 0)
		Gizmos::DrawWireCube(transform->GetGlobalPosition(), float3::one(), Color::Red());
	else if(collider_position == 2)
		Gizmos::DrawWireCube(transform->GetGlobalPosition(), float3::one(), Color::Green());
}
