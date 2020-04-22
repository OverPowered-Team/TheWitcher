#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ResourceBone.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBone.h"
#include "mmgr/mmgr.h"


ComponentBone::ComponentBone(GameObject* gameobject) : Component(gameobject)
{
	type = ComponentType::BONE;
}


ComponentBone::~ComponentBone()
{
	if (bone)
		bone->DecreaseReferences();

	bone = nullptr;
}

void ComponentBone::Draw()
{
	if (!debug_draw)
		return;

	//App->renderer3D->DrawSphere(gameobject->transform->GetPosition(), gameobject->transform->GetScale().x * 0.5f);
}

void ComponentBone::AddBone(ResourceBone* b)
{
	if (b)
		bone = b;
}

std::string ComponentBone::GetMeshName()
{
	return bone->mesh_name;
}

uint ComponentBone::GetBoneID()
{
	return bone->GetID();
}

ResourceBone* ComponentBone::GetBone()
{
	return bone;
}

void ComponentBone::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetString("BoneID", bone ? std::to_string(bone->GetID()).data() : std::to_string(0).data());
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentBone::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	u64 bone_ID = std::stoull(to_load->GetString("BoneID"));
	if (bone_ID != 0)
	{
		bone = (ResourceBone*)App->resources->GetResourceWithID(bone_ID);
		if (bone != nullptr)
			bone->IncreaseReferences();
	}
}
