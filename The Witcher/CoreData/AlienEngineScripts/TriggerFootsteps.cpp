#include "TriggerFootsteps.h"
#include "PlayerController.h"
#include "CameraMovement.h"
#include "..\..\MathGeoLib\include\Math\float4.h"

TriggerFootsteps::TriggerFootsteps() : Alien()
{
}

TriggerFootsteps::~TriggerFootsteps()
{
}

void TriggerFootsteps::Start()
{
	camera = Camera::GetCurrentCamera()->game_object_attached;
	cam_script = camera->GetComponent<CameraMovement>();
	emitter = game_object->GetComponent<ComponentAudioEmitter>();
}

void TriggerFootsteps::Update()
{
}

void TriggerFootsteps::OnTriggerEnter(ComponentCollider* collider)
{
	ComponentAudioEmitter* emitter = collider->game_object_attached->GetComponent<ComponentAudioEmitter>();

	if (emitter != nullptr)
		emitter->SetSwitchState("Material", GetNameByEnum(material_1).c_str());

	// Set player particle color
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
		collider->game_object_attached->GetComponent<PlayerController>()->OnTerrainEnter(
			terrain_particle_color, terrain_particle_color_final);
}

void TriggerFootsteps::OnTriggerExit(ComponentCollider* collider)
{
	//ComponentAudioEmitter* emitter = (ComponentAudioEmitter*)collider->game_object_attached->GetComponent(ComponentType::A_EMITTER);
	//emitter->SetSwitchState("Material", "Quiet");

	math::float4 white(1.f, 1.f, 1.f, 1.f);
	math::float4 white_less_alpha = white;
	white_less_alpha.w = 0.03f; 

	// Set player particle color back to default
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
		collider->game_object_attached->GetComponent<PlayerController>()->OnTerrainEnter(
			white, white_less_alpha);
}

std::string TriggerFootsteps::GetNameByEnum(GroundMaterial mat)
{
	std::string name;
	switch (mat)
	{
	case GroundMaterial::MUD:
		return name = "Mud";
		break;
	case GroundMaterial::SAND:
		return name = "Sand";
		break;
	case GroundMaterial::STONE:
		return name = "Stone";
		break;
	case GroundMaterial::VEGETATION:
		return name = "Vegetation";
		break;
	case GroundMaterial::WOOD:
		return name = "Wood";
		break;
	}
}
