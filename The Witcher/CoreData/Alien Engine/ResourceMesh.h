#pragma once

#include "Resource_.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "GameObject.h"
#include "Color.h"

class ResourceTexture;

class ResourceMesh : public Resource {

	friend class ModuleImporter;
	friend class ResourceModel;
	friend class ComponentDeformableMesh;
public:

	ResourceMesh();
	ResourceMesh(ResourceMesh* r_mesh);
	virtual ~ResourceMesh();

	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);

	void FreeMemory();
	bool LoadMemory();

	bool DeleteMetaData();

	void InitBuffers();

	void Reset();

public:
	// Vertex Array Object
	uint vao = 0u;
	// buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_normals = 0;
	uint id_uv = 0;
	uint id_weights = 0;
	uint id_bones = 0;
	uint id_tangents = 0;
	uint id_biTangents = 0;

	// buffers size
	uint num_index = 0;
	uint num_vertex = 0;
	uint num_faces = 0;
	// buffers
	uint* index = nullptr;

	float* vertex = nullptr;
	float* normals = nullptr;
	float* uv_cords = nullptr;
	float* center_point_normal = nullptr;
	float* center_point = nullptr;
	float* tangents = nullptr;
	float* biTangents = nullptr; 

	bool is_primitive = false;
	bool is_custom = true;

	

private:
	bool deformable = false;
	uint bytes_moved = 0;
	
	//Skinning with shaders
	float* weights = nullptr;
	int* bones_ID = nullptr;
};