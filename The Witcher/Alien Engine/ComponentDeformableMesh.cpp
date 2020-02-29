#include "Application.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ComponentBone.h"
#include "ComponentMaterial.h"
#include "ResourceMesh.h"
#include "ResourceBone.h"
#include "ComponentDeformableMesh.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ComponentDeformableMesh::ComponentDeformableMesh(GameObject* attach) : ComponentMesh(attach)
{
	type = ComponentType::DEFORMABLE_MESH;
	name = "Deformable Mesh";
}

ComponentDeformableMesh::~ComponentDeformableMesh()
{
	if (game_object_attached != nullptr && game_object_attached->HasComponent(ComponentType::MATERIAL))
	{
		static_cast<ComponentMaterial*>(game_object_attached->GetComponent(ComponentType::MATERIAL))->not_destroy = false;
	}
	if (original_mesh != nullptr && original_mesh->is_custom) {
		original_mesh->DecreaseReferences();
	}
	if (mesh)
	{
		delete mesh;
		mesh = nullptr;
	}
	//clear deformable mesh?
}

void ComponentDeformableMesh::AttachSkeleton(ComponentTransform* root)
{
	root_bone_id = root->game_object_attached->ID;

	//Duplicate mesh
	if (mesh)
	{
		ResourceMesh* tmp_mesh = new ResourceMesh(mesh);
		original_mesh = mesh;
		mesh = tmp_mesh;
	}
		
	AttachBone(root);
}

void ComponentDeformableMesh::AttachSkeleton()
{
	if (root_bone_id != 0)
		AttachSkeleton(App->objects->GetGameObjectByID(root_bone_id)->transform);
}

void ComponentDeformableMesh::AttachBone(ComponentTransform* bone_transform)
{
	ComponentBone* c_bone = (ComponentBone*)bone_transform->game_object_attached->GetComponent(ComponentType::BONE);

	if (c_bone)
		bones.push_back(c_bone);

	for each (GameObject* go in bone_transform->game_object_attached->GetChildren())
		AttachBone(go->transform);
}

void ComponentDeformableMesh::DeformMesh()
{
	OPTICK_EVENT();
	mesh->Reset();

	for (std::vector<ComponentBone*>::iterator it = bones.begin(); it != bones.end(); ++it)
	{
		ResourceBone* r_bone = (ResourceBone*)(*it)->GetBone();

		float4x4 matrix = (*it)->game_object_attached->transform->global_transformation;
		matrix = game_object_attached->transform->global_transformation.Inverted() * matrix;
		matrix = matrix * r_bone->matrix;

		for (uint i = 0; i < r_bone->num_weights; i++)
		{
			uint index = r_bone->vertex_ids[i];
			float3 original(&original_mesh->vertex[index * 3]);
			float3 vertex = matrix.TransformPos(original);

			mesh->vertex[index * 3] += vertex.x * r_bone->weights[i];
			mesh->vertex[index * 3 + 1] += vertex.y * r_bone->weights[i];
			mesh->vertex[index * 3 + 2] += vertex.z * r_bone->weights[i];

			if ((mesh->num_vertex) > 0)
			{
				vertex = matrix.TransformPos(float3(&mesh->normals[index*3]));
				mesh->normals[index * 3] += vertex.x * r_bone->weights[i];
				mesh->normals[index * 3 + 1] += vertex.y * r_bone->weights[i];
				mesh->normals[index * 3 + 2] += vertex.z * r_bone->weights[i];
			}
		}
	}
}

void ComponentDeformableMesh::DrawPolygon(ComponentCamera* camera)
{
	DeformMesh();

	//SKINNING STUFF
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, mesh->num_vertex * sizeof(float) * 3, mesh->vertex, GL_DYNAMIC_DRAW);

	if (mesh->normals)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
		glBufferData(GL_ARRAY_BUFFER, mesh->num_vertex * sizeof(float) * 3, mesh->normals, GL_DYNAMIC_DRAW);
	}
	/////////
	ComponentMesh::DrawPolygon(camera);
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
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetString("MeshID", original_mesh ? std::to_string(original_mesh->GetID()) : std::to_string(0));
	to_save->SetString("RootBoneID", root_bone_id != 0 ? std::to_string(root_bone_id) : std::to_string(0));
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
	root_bone_id = std::stoull(to_load->GetString("RootBoneID"));
	ID = std::stoull(to_load->GetString("ID"));
	u64 mesh_ID = std::stoull(to_load->GetString("MeshID"));
	if (mesh_ID != 0)
	{
		mesh = (ResourceMesh*)App->resources->GetResourceWithID(mesh_ID);
		if (mesh != nullptr)
			mesh->IncreaseReferences();
	}

	GenerateAABB();
	RecalculateAABB_OBB();
}

