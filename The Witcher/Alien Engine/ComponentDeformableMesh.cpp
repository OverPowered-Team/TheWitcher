#include "Application.h"
#include "Component.h"
#include "ComponentDeformableMesh.h"
#include "ComponentTransform.h"
#include "ModuleObjects.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "ComponentBone.h"
#include "ComponentMaterial.h"


#include "ResourceMesh.h"
#include "ResourceBone.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourceMaterial.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"
#include <stdio.h>
#include <iostream>

ComponentDeformableMesh::ComponentDeformableMesh(GameObject* attach) : ComponentMesh(attach)
{
	type = ComponentType::DEFORMABLE_MESH;
	name = "Deformable Mesh";

	//Change this to resource model so is only one new 
	bones_matrix = new float4x4[100];
	memset(bones_matrix, 0, sizeof(float) * 100 * 4 * 4);
	animate = 1;
}

ComponentDeformableMesh::~ComponentDeformableMesh()
{
	if (game_object_attached != nullptr && game_object_attached->HasComponent(ComponentType::MATERIAL))
	{
		static_cast<ComponentMaterial*>(game_object_attached->GetComponent(ComponentType::MATERIAL))->not_destroy = false;
	}
	if (mesh)
	{
		mesh->DecreaseReferences();
		mesh = nullptr;
	}
	//clear bones matrix :)
	delete[] bones_matrix;
}

void ComponentDeformableMesh::AttachSkeleton(ComponentTransform* root)
{
	root_bone = root->game_object_attached;
		
	AttachBone(root);
	
	material = (ComponentMaterial*)game_object_attached->GetComponent(ComponentType::MATERIAL);

	SendWeightsAndID();

}

void ComponentDeformableMesh::AttachSkeleton()
{
	if (rootID != 0) {
		root_bone = App->objects->GetGameObjectByID(rootID);
		if (root_bone != nullptr)
			AttachSkeleton(root_bone->transform);
	}
}

void ComponentDeformableMesh::AttachBone(ComponentTransform* bone_transform)
{
	if (mesh == nullptr) {
		return;
	}
	std::vector<ComponentBone*> c_bones = bone_transform->game_object_attached->GetComponents<ComponentBone>();

	if (c_bones.size() > 0)
	{
		for each (ComponentBone* c_bone in c_bones)
		{
			if (c_bone->GetMeshName() == mesh->name)
				bones.push_back(c_bone);
		}
	}
		

	for each (GameObject* go in bone_transform->game_object_attached->GetChildren())
		AttachBone(go->transform);
}

void ComponentDeformableMesh::UpdateBonesMatrix()
{
	OPTICK_EVENT(); 

	uint i = 0;
	for (std::vector<ComponentBone*>::iterator it = bones.begin(); it != bones.end(); ++it, ++i)
	{
		ResourceBone* r_bone = (*it)->GetBone();
		math::float4x4 boneGlobalMatrix = (*it)->game_object_attached->transform->global_transformation;
		math::float4x4 meshMatrix = game_object_attached->transform->global_transformation.Inverted();
		math::float4x4 boneTransform = meshMatrix * boneGlobalMatrix * r_bone->matrix;
		
		bones_matrix[i] = boneTransform;
	}
}

void ComponentDeformableMesh::DrawScene(ComponentCamera* camera)
{
	OPTICK_EVENT();

	if (IsEnabled())
	{
		if (!wireframe)
			DrawPolygon(camera);
		/*if ((selected || parent_selected) && App->objects->outline)
			mesh->DrawOutLine();*/
		if (view_mesh || wireframe)
			DrawMesh();
		if (view_vertex_normals)
			DrawVertexNormals();
		if (view_face_normals)
			DrawFaceNormals();
		if (draw_AABB)
			DrawGlobalAABB(camera);
		if (draw_OBB)
			DrawOBB(camera);
	}
}

void ComponentDeformableMesh::DrawGame(ComponentCamera* camera)
{
	OPTICK_EVENT();

	if (IsEnabled())
	{
		DrawPolygon(camera);
	}
}


void ComponentDeformableMesh::DrawPolygon(ComponentCamera* camera)
{
	OPTICK_EVENT();
	if (mesh == nullptr || mesh->id_index <= 0 || material == nullptr)
		return;

	UpdateBonesMatrix();

	ComponentMesh::DrawPolygon(camera);

}

void ComponentDeformableMesh::PreDrawPolygonForShadows(ComponentCamera* camera, const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position)
{
	OPTICK_EVENT();
	if (mesh == nullptr || mesh->id_index <= 0 || material == nullptr)
		return;

	UpdateBonesMatrix();

	ComponentMesh::PreDrawPolygonForShadows(camera, ViewMat, ProjMatrix, position);
}

void ComponentDeformableMesh::SetUniforms(ResourceMaterial* resource_material, ComponentCamera* camera)
{
	ComponentMesh::SetUniforms(resource_material, camera);
	resource_material->used_shader->SetUniformMat4f("gBones", bones_matrix, bones.size());
}

void ComponentDeformableMesh::SetShadowUniforms(ResourceMaterial* resource_material, ComponentCamera* camera,const float4x4& ViewMat, const float4x4& ProjMatrix, const float3& position)
{
	ComponentMesh::SetShadowUniforms(resource_material, camera, ViewMat, ProjMatrix, position);
	resource_material->simple_depth_shader->SetUniformMat4f("gBones", bones_matrix, bones.size());
}

void ComponentDeformableMesh::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("ViewMesh", view_mesh);
	to_save->SetBoolean("Wireframe", wireframe);
	to_save->SetBoolean("ViewVertexNormals", view_vertex_normals);
	to_save->SetBoolean("ViewFaceNormals", view_face_normals);
	to_save->SetBoolean("DrawAABB", draw_AABB);
	to_save->SetBoolean("DrawOBB", draw_OBB);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetString("MeshID", mesh ? std::to_string(mesh->GetID()).data() : std::to_string(0).data());
	to_save->SetString("RootBoneID", root_bone != nullptr ? std::to_string(root_bone->ID).data() : std::to_string(0).data());
	to_save->SetBoolean("Enabled", enabled);
}

void ComponentDeformableMesh::LoadComponent(JSONArraypack* to_load)
{
	view_mesh = to_load->GetBoolean("ViewMesh");
	wireframe = to_load->GetBoolean("Wireframe");
	view_vertex_normals = to_load->GetBoolean("ViewVertexNormals");
	view_face_normals = to_load->GetBoolean("ViewFaceNormals");
	draw_AABB = to_load->GetBoolean("DrawAABB");
	draw_OBB = to_load->GetBoolean("DrawOBB");
	enabled = to_load->GetBoolean("Enabled");
	rootID = std::stoull(to_load->GetString("RootBoneID"));
	ID = std::stoull(to_load->GetString("ID"));
	u64 mesh_ID = std::stoull(to_load->GetString("MeshID"));
	if (mesh_ID != 0)
	{
		mesh = (ResourceMesh*)App->resources->GetResourceWithID(mesh_ID);
		if (mesh != nullptr)
			mesh->IncreaseReferences();
	}

	GenerateLocalAABB();
	RecalculateAABB_OBB();
}

//TODO change this to execute this code only one time
//When loading resouce model
void ComponentDeformableMesh::SendWeightsAndID()
{
	if (mesh == nullptr) {
		return;
	}

	if (mesh->weights != nullptr && mesh->bones_ID != nullptr)
		return;

	int bone_id = 0;
	//Genereting array of weights and bones_ID
	if (mesh->weights == nullptr && mesh->bones_ID == nullptr)
	{
		mesh->weights = new float[mesh->num_vertex * 4];
		mesh->bones_ID = new int[mesh->num_vertex * 4];

		memset(mesh->weights, 0, sizeof(float) * mesh->num_vertex * 4);
		memset(mesh->bones_ID, 0, sizeof(int) * mesh->num_vertex * 4);
	}

	for (std::vector<ComponentBone*>::iterator component_bone = bones.begin();
		component_bone != bones.end(); component_bone++ , bone_id++)
	{
		FillWeights(bone_id, (*component_bone));
	}
	glBindVertexArray(mesh->vao);


	if (mesh->id_bones == 0)
	{
		glGenBuffers(1, &mesh->id_bones);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_bones);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 4, mesh->bones_ID, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	if (mesh->id_weights == 0)
	{
		glGenBuffers(1, &mesh->id_weights);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_weights);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 4, mesh->weights, GL_STATIC_DRAW);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);
}

void ComponentDeformableMesh::FillWeights(int bone_ID, ComponentBone* component_bone)
{
	ResourceBone* bone = component_bone->GetBone();
	for (int i = 0; i < bone->num_weights; i++)
	{
		int vertex_id = bone->vertex_ids[i];
		for (int j = vertex_id * 4; j < (vertex_id * 4) + 4; j++)
		{
			if (mesh->weights[j] == 0.0f)
			{
				mesh->weights[j] = bone->weights[i];
				mesh->bones_ID[j] = bone_ID;
				break;
			}
		}
	}
}

