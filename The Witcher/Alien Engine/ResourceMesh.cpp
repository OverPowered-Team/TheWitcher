#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentBone.h"
#include "ModuleResources.h"
#include "ComponentDeformableMesh.h"
#include "ResourceTexture.h"
#include "ResourceBone.h"
#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh() : Resource()
{
	type = ResourceType::RESOURCE_MESH;
}

ResourceMesh::ResourceMesh(ResourceMesh* r_mesh)
{
	num_index = r_mesh->num_index;
	num_vertex = r_mesh->num_vertex;
	num_faces = r_mesh->num_faces;

	uint bytes = sizeof(float) * num_vertex * 3;
	if (num_vertex > 0) {
		vertex = new float[num_vertex * 3];
		memcpy(vertex, r_mesh->vertex, bytes);
	}

	bytes = sizeof(uint) * num_index;
	if (num_index > 0) {
		index = new uint[num_index];
		memcpy(index, r_mesh->index, bytes);
	}

	// normals
	if (r_mesh->normals) {
		bytes = sizeof(float) * num_vertex * 3;
		normals = new float[num_vertex * 3];
		memcpy(normals, r_mesh->normals, bytes);

		bytes = sizeof(float) * num_faces * 3;
		center_point = new float[num_faces * 3];
		memcpy(center_point, r_mesh->center_point, bytes);

		bytes = sizeof(float) * num_faces * 3;
		center_point_normal = new float[num_faces * 3];
		memcpy(center_point_normal, r_mesh->center_point_normal, bytes);
	}

	// uv
	if (r_mesh->uv_cords) {
		bytes = sizeof(float) * num_vertex * 3;
		uv_cords = new float[num_vertex * 3];
		memcpy(uv_cords, r_mesh->uv_cords, bytes);
	}

	// Tangents and bitangents 
	if (r_mesh->tangents) {
		bytes = sizeof(float) * num_vertex * 3;
		tangents = new float[num_vertex * 3];
		memcpy(tangents, r_mesh->tangents, bytes);
	}

	if (r_mesh->biTangents) {
		bytes = sizeof(float) * num_vertex * 3;
		biTangents = new float[num_vertex * 3];
		memcpy(biTangents, r_mesh->biTangents, bytes);
	}

	if (num_vertex != 0) {
		InitBuffers();
	}
}

ResourceMesh::~ResourceMesh()
{	
	FreeMemory();
}

bool ResourceMesh::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	meta_data_path = std::string(LIBRARY_MESHES_FOLDER + std::to_string(ID) + ".alienMesh");

	uint ranges[8] = { num_index, num_vertex, num_faces, (normals != nullptr) ? true : false, 
		(uv_cords != nullptr) ? true : false, name.size(), (tangents != nullptr) ? true : false,  (biTangents != nullptr) ? true : false };

	uint uv_size = (uv_cords != nullptr) ? sizeof(float) * num_vertex * 3 : 0;
	uint normals_size = (normals != nullptr) ? sizeof(float) * num_vertex * 3 + sizeof(float) * num_faces * 3 +
		sizeof(float) * num_faces * 3 : 0;
	uint vertex_size = sizeof(float) * num_vertex * 3;
	uint index_size = sizeof(uint) * num_index;
	uint tangents_size = sizeof(float) * num_vertex * 3;
	uint biTangents_size = sizeof(float) * num_vertex * 3;

	uint size = sizeof(ranges) + name.size() +
		sizeof(float) * 4 + sizeof(float) * 3 + sizeof(float) * 4 + sizeof(float) * 3 + vertex_size +
		index_size + normals_size + uv_size + sizeof(bool) + tangents_size + biTangents_size;

	char* data = new char[size]; 
	char* cursor = data;

	uint bytes = sizeof(ranges); 
	memcpy(cursor, ranges, bytes);
	cursor += bytes; 
	bytes_moved += bytes;

	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;
	bytes_moved += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &deformable, bytes);
	cursor += bytes;
	bytes_moved += bytes;

	if (num_vertex > 0) {
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, vertex, bytes);
		cursor += bytes;
	}

	if (num_index > 0) {
		bytes = sizeof(uint) * num_index;
		memcpy(cursor, index, bytes);
		cursor += bytes;
	}

	if (normals != nullptr) {
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, normals, bytes);
		cursor += bytes;

		bytes = sizeof(float) * num_faces * 3;
		memcpy(cursor, center_point, bytes);
		cursor += bytes;

		bytes = sizeof(float) * num_faces * 3;
		memcpy(cursor, center_point_normal, bytes);
		cursor += bytes;
	}

	if (uv_cords != nullptr) {
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, uv_cords, bytes);
		cursor += bytes;
	}

	if (tangents != nullptr)
	{
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, tangents, bytes);
		cursor += bytes;
	}

	if (biTangents != nullptr)
	{
		bytes = sizeof(float) * num_vertex * 3;
		memcpy(cursor, biTangents, bytes);
	}

	App->file_system->Save(meta_data_path.data(), data, size);

	delete[] data;

	return true;
}

bool ResourceMesh::ReadBaseInfo(const char* meta_file_path)
{
	meta_data_path = std::string(meta_file_path);
	ID = std::stoull(App->file_system->GetBaseFileName(meta_file_path));

	char* data = nullptr;
	uint size = App->file_system->Load(meta_data_path.data(), &data);

	if (size > 0) {
		char* cursor = data;
		bytes_moved = 0;

		uint ranges[8];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes;
		bytes_moved += bytes;

		char* m_name = new char[ranges[5] + 1];
		bytes = sizeof(char) * ranges[5];
		memcpy(m_name, cursor, bytes);
		cursor += bytes;
		bytes_moved += bytes;
		m_name[ranges[5]] = '\0';
		name = std::string(m_name);
		delete[] m_name;

		bytes = sizeof(bool);
		memcpy(&deformable, cursor, bytes);
		cursor += bytes;
		bytes_moved += bytes;

		App->resources->AddResource(this);
		delete[] data;

		return true;
	}
	else {
		return false;
	}
}

void ResourceMesh::FreeMemory()
{
	if (vao != 0)
		glDeleteVertexArrays(1, &vao);
	if (id_vertex != 0)
		glDeleteBuffers(1, &id_vertex);
	if (id_index != 0)
		glDeleteBuffers(1, &id_index);
	if (id_normals != 0)
		glDeleteBuffers(1, &id_normals);
	if (id_uv != 0)
		glDeleteBuffers(1, &id_uv);
	if(id_weights!=0)
		glDeleteBuffers(1, &id_weights);
	if (id_bones != 0)
		glDeleteBuffers(1, &id_bones);
	if (id_tangents != 0)
		glDeleteBuffers(1, &id_tangents);
	if (id_biTangents != 0)
		glDeleteBuffers(1, &id_biTangents);

	id_vertex = 0;
	id_index = 0;
	id_normals = 0;
	id_uv = 0;
	id_weights = 0;
	id_bones = 0;
	id_tangents = 0;
	id_biTangents = 0;
	references = 0;

	num_vertex = 0;
	num_index = 0;
	num_faces = 0;


	if (index != nullptr) {
		delete[] index;
		index = nullptr;
	}
	if (vertex != nullptr) {
		delete[] vertex;
		vertex = nullptr;
	}
	if (normals != nullptr) {
		delete[] normals;
		normals = nullptr;
	}
	if (uv_cords != nullptr) {
		delete[] uv_cords;
		uv_cords = nullptr;
	}
	if (center_point_normal != nullptr) {
		delete[] center_point_normal;
		center_point_normal = nullptr;
	}
	if (center_point != nullptr) {
		delete[] center_point;
		center_point = nullptr;
	}
	if (weights != nullptr)
	{
		delete[] weights;
		weights = nullptr;
	}
	if (bones_ID != nullptr)
	{
		delete[] bones_ID;
		bones_ID = nullptr;
	}
	if (tangents != nullptr)
	{
		delete[] tangents;
		tangents = nullptr;
	}
	if (biTangents != nullptr)
	{
		delete[] biTangents;
		biTangents = nullptr;
	}

}

bool ResourceMesh::LoadMemory()
{	
	if (num_vertex != 0) {
		return true;
	}

	char* data = nullptr;
	uint size = App->file_system->Load(meta_data_path.data(), &data);

	if (size > 0) {
		char* cursor = data;

		uint ranges[6];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes_moved;

		num_index = ranges[0];
		num_vertex = ranges[1];
		num_faces = ranges[2];

		bytes = sizeof(float) * num_vertex * 3;
		if (num_vertex > 0) {
			vertex = new float[num_vertex * 3];
			memcpy(vertex, cursor, bytes);
		}
		cursor += bytes;

		bytes = sizeof(uint) * num_index;
		if (num_index > 0) {
			index = new uint[num_index];
			memcpy(index, cursor, bytes);
		}
		cursor += bytes;

		// normals
		if (ranges[3]) {
			bytes = sizeof(float) * num_vertex * 3;
			normals = new float[num_vertex * 3];
			memcpy(normals, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float) * num_faces * 3;
			center_point = new float[num_faces * 3];
			memcpy(center_point, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float) * num_faces * 3;
			center_point_normal = new float[num_faces * 3];
			memcpy(center_point_normal, cursor, bytes);
			cursor += bytes;
		}

		// uv
		if (ranges[4]) {
			bytes = sizeof(float) * num_vertex * 3;
			uv_cords = new float[num_vertex * 3];
			memcpy(uv_cords, cursor, bytes);
			cursor += bytes;
		}

		// Tangents 
		if (ranges[6])
		{
			bytes = sizeof(float) * num_vertex * 3;
			tangents = new float[num_vertex * 3];
			memcpy(tangents, cursor, bytes);
			cursor += bytes;
		}

		// Bitangents 
		if (ranges[7])
		{
			bytes = sizeof(float) * num_vertex * 3;
			biTangents = new float[num_vertex * 3];
			memcpy(biTangents, cursor, bytes);
		}

		if (num_vertex != 0) {
			InitBuffers();
		}
		else {
			--references;
		}
		delete[] data;
		return true;
	}
	else {
		delete[] data;
		return false;
	}
}

bool ResourceMesh::DeleteMetaData()
{
	remove(meta_data_path.data());

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end())
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceMesh::InitBuffers()
{
	// VAO
	glGenVertexArrays(1, &vao);


	glBindVertexArray(vao);

	// Vertex 
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(0);

	// UVS 
	if (uv_cords != nullptr) {
		// UV
		glGenBuffers(1, &id_uv);
		glBindBuffer(GL_ARRAY_BUFFER, id_uv);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, uv_cords, GL_STATIC_DRAW);	// This should be size 2

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);										// This should be size 2
		glEnableVertexAttribArray(1);
	}

	// Normals
	if (normals != nullptr) {
		// normals
		glGenBuffers(1, &id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}

	// Tangents
	if (tangents != nullptr)
	{
		glGenBuffers(1, &id_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, id_tangents);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, tangents, GL_STATIC_DRAW);

		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(5);
	}

	// Bitangents
	if (biTangents != nullptr)
	{
		glGenBuffers(1, &id_biTangents);
		glBindBuffer(GL_ARRAY_BUFFER, id_biTangents);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, biTangents, GL_STATIC_DRAW);

		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(6);
	}

	// index
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ResourceMesh::Reset()
{
	memset(vertex, 0, sizeof(float) * num_vertex * 3);

	if (normals > 0)
	{
		memset(normals, 0, sizeof(float) * num_vertex * 3);
	}
}
