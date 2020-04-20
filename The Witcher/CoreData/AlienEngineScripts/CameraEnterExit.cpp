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
	script_parent = game_object->GetComponentInParent<TriggerCamera>();
}

void CameraEnterExit::OnTriggerEnter(ComponentCollider* collider)
{
	PlayerController* player = collider->game_object_attached->GetComponent<PlayerController>();
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
