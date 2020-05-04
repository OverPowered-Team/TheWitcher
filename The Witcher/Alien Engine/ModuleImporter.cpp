#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleObjects.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#include "stb_image.h"
#include "FreeImage/src/FreeImage.h"

#pragma comment ( lib, "FreeImage/lib/FreeImage.lib ")

#include "ModuleUI.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentParticleSystem.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "ModuleFileSystem.h"

#include "ModuleResources.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "PanelProject.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourceAnimation.h"
#include "ResourceBone.h"
#include "ResourceMaterial.h"
#include "ResourceFont.h"

#include "ReturnZ.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include "mmgr/mmgr.h"
#include "FreeType/include/freetype/freetype.h"

#include "Optick/include/optick.h"

#define DAE_FPS 30

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
	name = "Importer";
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();
	LOG_ENGINE("Initing Devil");

	if (FT_Init_FreeType(&library))
	{
		LOG_ENGINE("Error when it's initialization FreeType");
	}
	else
		LOG_ENGINE("FreeType initialized!");

	return true;
}

bool ModuleImporter::Start()
{

	return true;
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	FT_Done_FreeType(library);

	return true;
}

bool ModuleImporter::LoadModelFile(const char *path, const char *extern_path)
{
	OPTICK_EVENT();
	bool ret = true;

	LOG_ENGINE("Loading %s", path);

	// if this file has been already imported just load the .alienModel
	Resource *model = nullptr;
	if (!App->resources->Exists(path, &model))
	{

		const aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
													  aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes | aiProcess_LimitBoneWeights);

		if (scene != nullptr)
		{
			InitScene(path, scene, extern_path);
			LOG_ENGINE("Succesfully loaded %s", path);
		}
		else
		{
			ret = false;
			LOG_ENGINE("Error loading model %s", path);
			LOG_ENGINE("Error type: %s", aiGetErrorString());
		}
		aiReleaseImport(scene);
		if(ret)
			App->resources->AddNewFileNode(path, true);
	}
	else
	{
		App->resources->CreateNewModelInstanceOf(model->GetLibraryPath());
	}

	return ret;
}

void ModuleImporter::InitScene(const char *path, const aiScene *scene, const char *extern_path)
{
	OPTICK_EVENT();
	model = new ResourceModel();
	model->name = App->file_system->GetBaseFileName(path);
	model->path = std::string(path);

	//Import meshes & bones
	if (scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			//Import mesh here.
			LoadMesh(scene->mMeshes[i]);

			//Import bones of mesh
			if (scene->mMeshes[i]->HasBones())
			{
				model->meshes_attached[i]->deformable = true;
				for (int j = 0; j < scene->mMeshes[i]->mNumBones; ++j)
				{
					LoadBone(scene->mMeshes[i]->mBones[j], model->meshes_attached[i]->name);
				}
			}
		}
	}

	if (scene->HasMaterials())
	{
		for (uint i = 0; i < scene->mNumMaterials; ++i)
		{
			LoadMaterials(scene->mMaterials[i], extern_path);
		}
	}

	// Import animations
	if (scene->HasAnimations())
	{
		for (int i = 0; i < scene->mNumAnimations; ++i)
		{
			LoadAnimation(scene->mAnimations[i]);
		}
	}

	// start recursive function to all nodes
	for (uint i = 0; i < scene->mRootNode->mNumChildren; ++i)
	{
		LoadNode(scene->mRootNode->mChildren[i], scene, 1);
	}

	// create the meta data files like .alien
	if (model->CreateMetaData())
	{
		App->resources->AddResource(model);
		if (App->ui->panel_project != nullptr) {
			App->ui->panel_project->RefreshAllNodes();
			model->ConvertToGameObjects();
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_OBJECT, App->objects->GetRoot(false)->children.back());
		}
	}

	model = nullptr;
}

void ModuleImporter::LoadAnimation(const aiAnimation *anim)
{
	OPTICK_EVENT();
	bool is_dae = anim->mTicksPerSecond == 1;

	ResourceAnimation* resource_animation = new ResourceAnimation();
	resource_animation->name = std::string(anim->mName.C_Str()) ==  "" ? "Take 001" : anim->mName.C_Str();
	resource_animation->ticks_per_second = is_dae ? DAE_FPS : anim->mTicksPerSecond;
	resource_animation->num_channels = anim->mNumChannels;
	resource_animation->channels = new ResourceAnimation::Channel[resource_animation->num_channels];
	resource_animation->start_tick = 0;
	resource_animation->end_tick = is_dae ? anim->mDuration * DAE_FPS : anim->mDuration;
	resource_animation->max_tick = resource_animation->end_tick;

	for (uint i = 0u; i < resource_animation->num_channels; ++i)
	{
		ResourceAnimation::Channel &channel = resource_animation->channels[i];
		channel.name = anim->mChannels[i]->mNodeName.C_Str();

		channel.num_position_keys = anim->mChannels[i]->mNumPositionKeys;
		channel.num_scale_keys = anim->mChannels[i]->mNumScalingKeys;
		channel.num_rotation_keys = anim->mChannels[i]->mNumRotationKeys;

		channel.position_keys = new KeyAnimation<float3>[channel.num_position_keys]();
		channel.scale_keys = new KeyAnimation<float3>[channel.num_scale_keys]();
		channel.rotation_keys = new KeyAnimation<Quat>[channel.num_rotation_keys]();

		//Load position keys
		for (uint j = 0; j < channel.num_position_keys; j++)
		{
			channel.position_keys[j].value.Set(anim->mChannels[i]->mPositionKeys[j].mValue.x, anim->mChannels[i]->mPositionKeys[j].mValue.y,
											   anim->mChannels[i]->mPositionKeys[j].mValue.z);
			channel.position_keys[j].time = is_dae ? std::round(anim->mChannels[i]->mPositionKeys[j].mTime * DAE_FPS) : anim->mChannels[i]->mPositionKeys[j].mTime;
		}

		//Load scaling keys
		for (uint j = 0; j < channel.num_scale_keys; j++)
		{
			channel.scale_keys[j].value.Set(anim->mChannels[i]->mScalingKeys[j].mValue.x, anim->mChannels[i]->mScalingKeys[j].mValue.y,
											anim->mChannels[i]->mScalingKeys[j].mValue.z);
			channel.scale_keys[j].time = is_dae ? std::round(anim->mChannels[i]->mScalingKeys[j].mTime * DAE_FPS) : anim->mChannels[i]->mScalingKeys[j].mTime;
		}

		//Load rotation keys
		for (uint j = 0; j < channel.num_rotation_keys; j++)
		{
			channel.rotation_keys[j].value.Set(anim->mChannels[i]->mRotationKeys[j].mValue.x, anim->mChannels[i]->mRotationKeys[j].mValue.y,
											   anim->mChannels[i]->mRotationKeys[j].mValue.z, anim->mChannels[i]->mRotationKeys[j].mValue.w);
			channel.rotation_keys[j].time = is_dae ? std::round(anim->mChannels[i]->mRotationKeys[j].mTime * DAE_FPS) : anim->mChannels[i]->mRotationKeys[j].mTime;
		}
	}

	App->resources->AddResource(resource_animation);
	model->animations_attached.push_back(resource_animation);
}

void ModuleImporter::LoadBone(const aiBone *bone, std::string mesh_name)
{
	OPTICK_EVENT();
	ResourceBone* r_bone = new ResourceBone();

	r_bone->name = bone->mName.C_Str();
	r_bone->mesh_name = mesh_name;
	r_bone->matrix = float4x4(float4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1),
							  float4(bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2),
							  float4(bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3),
							  float4(bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4));

	r_bone->num_weights = bone->mNumWeights;
	r_bone->weights = new float[r_bone->num_weights];
	r_bone->vertex_ids = new uint[r_bone->num_weights];

	for (uint i = 0; i < r_bone->num_weights; i++)
	{
		memcpy(&r_bone->weights[i], &bone->mWeights[i].mWeight, sizeof(float));
		memcpy(&r_bone->vertex_ids[i], &bone->mWeights[i].mVertexId, sizeof(uint));
	}

	App->resources->AddResource(r_bone);
	model->bones_attached.push_back(r_bone);
}

void ModuleImporter::LoadMesh(const aiMesh *mesh)
{
	OPTICK_EVENT();
	ResourceMesh* ret = new ResourceMesh();
	
	// get vertex
	ret->vertex = new float[mesh->mNumVertices * 3];
	memcpy(ret->vertex, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3);
	ret->num_vertex = mesh->mNumVertices;

	// get index
	if (mesh->HasFaces())
	{
		ret->num_index = mesh->mNumFaces * 3;
		ret->index = new uint[ret->num_index]; // assume each face is a triangle
		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				uint non[3] = {0, 0, 0};
				memcpy(&ret->index[i * 3], non, 3 * sizeof(uint));
				LOG_ENGINE("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&ret->index[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}
	// get normals
	if (mesh->HasNormals())
	{
		ret->normals = new float[mesh->mNumVertices * 3];
		memcpy(ret->normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);

		ret->center_point_normal = new float[mesh->mNumFaces * 3];
		ret->center_point = new float[mesh->mNumFaces * 3];
		ret->num_faces = mesh->mNumFaces;
		for (uint i = 0; i < ret->num_index; i += 3)
		{
			uint index1 = ret->index[i] * 3;
			uint index2 = ret->index[i + 1] * 3;
			uint index3 = ret->index[i + 2] * 3;

			float3 x0(ret->vertex[index1], ret->vertex[index1 + 1], ret->vertex[index1 + 2]);
			float3 x1(ret->vertex[index2], ret->vertex[index2 + 1], ret->vertex[index2 + 2]);
			float3 x2(ret->vertex[index3], ret->vertex[index3 + 1], ret->vertex[index3 + 2]);

			float3 v0 = x0 - x2;
			float3 v1 = x1 - x2;
			float3 n = v0.Cross(v1);

			float3 normalized = n.Normalized();

			ret->center_point[i] = (x0.x + x1.x + x2.x) / 3;
			ret->center_point[i + 1] = (x0.y + x1.y + x2.y) / 3;
			ret->center_point[i + 2] = (x0.z + x1.z + x2.z) / 3;

			ret->center_point_normal[i] = normalized.x;
			ret->center_point_normal[i + 1] = normalized.y;
			ret->center_point_normal[i + 2] = normalized.z;
		}
	}
	// get UV
	if (mesh->HasTextureCoords(0))
	{
		ret->uv_cords = new float[mesh->mNumVertices * 3];
		memcpy(ret->uv_cords, (float *)mesh->mTextureCoords[0], sizeof(float) * mesh->mNumVertices * 3);
	}

	if (mesh->HasTangentsAndBitangents())
	{
		ret->tangents = new float[mesh->mNumVertices * 3];
		memcpy(ret->tangents, (float*)mesh->mTangents, sizeof(float) * mesh->mNumVertices * 3);

		ret->biTangents = new float[mesh->mNumVertices * 3];
		memcpy(ret->biTangents, (float*)mesh->mBitangents, sizeof(float) * mesh->mNumVertices * 3);
	}

	ret->name = std::string(mesh->mName.C_Str());
	ret->InitBuffers();
	App->resources->AddResource(ret);
	model->meshes_attached.push_back(ret);
}

void ModuleImporter::LoadNode(const aiNode *node, const aiScene *scene, uint nodeNum)
{
	OPTICK_EVENT();
	aiMatrix4x4 mat;
	while (std::string(node->mName.C_Str()).find("_$AssimpFbx$_") != std::string::npos)
	{
		mat = mat * node->mTransformation;
		node = node->mChildren[0];
	}
	if (mat.IsIdentity())
		mat = node->mTransformation;

	ModelNode model_node;
	model_node.name = std::string(node->mName.C_Str());

	const aiNode* pNode = node;
	while (std::string(pNode->mParent->mName.C_Str()).find("_$AssimpFbx$_") != std::string::npos) {
		pNode = pNode->mParent;
	}
	model_node.parent_name = (pNode->mParent == scene->mRootNode) ? model->name : std::string(pNode->mParent->mName.C_Str());

	model_node.parent_num = nodeNum;
	model_node.node_num = nodeNum + 1;
	if (node->mNumMeshes == 1)
	{
		model_node.mesh = node->mMeshes[0];
		model_node.material = scene->mMeshes[node->mMeshes[0]]->mMaterialIndex;
	}
	else if (node->mNumMeshes > 1)
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			ModelNode nodeMesh;
			nodeMesh.name = std::string(node->mName.C_Str() + std::to_string(i));
			nodeMesh.mesh = node->mMeshes[i];
			nodeMesh.material = scene->mMeshes[node->mMeshes[i]]->mMaterialIndex;
			nodeMesh.parent_name = model_node.name;
			nodeMesh.node_num = nodeNum + 2;
			nodeMesh.parent_num = nodeNum + 1;
			model->model_nodes.push_back(nodeMesh);
		}
	}
	for (uint i = 0; i < model->bones_attached.size(); ++i)
	{
		if (model->bones_attached[i]->name == model_node.name)
		{
			model_node.bones.push_back(i);
		}
	}
	aiVector3D pos, scale;
	aiQuaternion rot;
	mat.Decompose(scale, rot, pos);
	model_node.pos = float3(pos.x, pos.y, pos.z);
	model_node.scale = float3(scale.x, scale.y, scale.z);
	model_node.rot = Quat(rot.x, rot.y, rot.z, rot.w);

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		LoadNode(node->mChildren[i], scene, nodeNum + 1);
	}

	model->model_nodes.push_back(model_node);
}

void ModuleImporter::LoadMaterials(aiMaterial *material, const char *extern_path)
{
	OPTICK_EVENT();
	ResourceMaterial* mat = new ResourceMaterial();
	mat->SetName(material->GetName().C_Str());
	aiColor4D col;
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, col))
	{
		mat->color.x = col.r;
		mat->color.y = col.g;
		mat->color.z = col.b;
		mat->color.w = col.a;
	}

	LoadModelTexture(material, mat, aiTextureType_DIFFUSE, TextureType::DIFFUSE, extern_path);
	LoadModelTexture(material, mat, aiTextureType_SPECULAR, TextureType::SPECULAR, extern_path);
	LoadModelTexture(material, mat, aiTextureType_NORMALS, TextureType::NORMALS, extern_path);

	model->materials_attached.push_back(mat);
}

void ModuleImporter::LoadModelTexture(const aiMaterial *material, ResourceMaterial *mat, aiTextureType assimp_type, TextureType type, const char *extern_path)
{
	OPTICK_EVENT();
	aiString ai_path;
	if (AI_SUCCESS == material->GetTexture(assimp_type, 0, &ai_path))
	{
		std::string name = ai_path.C_Str();
		App->file_system->NormalizePath(name);
		ResourceTexture *tex = (ResourceTexture *)App->resources->GetTextureByName(name.data());
		if (tex != nullptr)
		{
			mat->textures[(uint)type].first = tex->GetID();
			mat->textures[(uint)type].second = tex;
		}
		else if (extern_path != nullptr)
		{
			std::string normExtern(extern_path);
			App->file_system->NormalizePath(normExtern);
			std::string textureName = App->file_system->GetBaseFileNameWithExtension(name.data());
			App->file_system->NormalizePath(textureName);
			std::string texturePath = App->file_system->GetCurrentHolePathFolder(normExtern) + textureName;

			while (texturePath[0] == '.' || texturePath[1] == '/') {
				texturePath.erase(texturePath.begin());
			}

			if (std::experimental::filesystem::exists(texturePath))
			{
				std::string assets_path = TEXTURES_FOLDER + textureName;
				App->file_system->CopyFromOutsideFS(texturePath.data(), assets_path.data());
				tex = new ResourceTexture(assets_path.data());

				tex->CreateMetaData();
				App->resources->AddNewFileNode(assets_path, true);

				mat->textures[(uint)type].first = tex->GetID();
				mat->textures[(uint)type].second = tex;
			}
		}
	}
}

ResourceTexture *ModuleImporter::LoadTextureFile(const char *path, bool has_been_dropped, bool is_custom)
{
	OPTICK_EVENT();
	ResourceTexture* texture = nullptr;

	if (!has_been_dropped && !App->file_system->Exists(path))
	{
		return nullptr;
	}

	Resource *tex = nullptr;
	if (App->resources->Exists(path, &tex))
	{

		std::string meta_path_in_assets = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(meta_path_in_assets.data());

		texture = (ResourceTexture *)App->resources->GetResourceWithID(ID);

		if (has_been_dropped && !App->objects->GetSelectedObjects().empty())
		{
			ApplyTextureToSelectedObject(texture);
		}
		LOG_ENGINE("This texture was already loaded");

		return texture;
	}
	else
	{
		texture = new ResourceTexture(path);

		texture->CreateMetaData();
		App->resources->AddNewFileNode(path, true);

		if (has_been_dropped && !App->objects->GetSelectedObjects().empty())
		{
			ApplyTextureToSelectedObject(texture);
		}
	}

	return texture;
}

ResourceTexture *ModuleImporter::LoadEngineTexture(const char *path)
{
	OPTICK_EVENT();
	ResourceTexture* texture = nullptr;

	ILuint new_image_id = 0;
	ilGenImages(1, &new_image_id);
	ilBindImage(new_image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilLoadImage(path))
	{
		iluFlipImage();
		texture = new ResourceTexture(path, ilutGLBindTexImage(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		texture->is_custom = false;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		App->resources->AddResource(texture);

		LOG_ENGINE("Texture successfully loaded: %s", path);
	}
	else
	{
		LOG_ENGINE("Error while loading image in %s", path);
		LOG_ENGINE("Error: %s", ilGetString(ilGetError()));
	}

	ilDeleteImages(1, &new_image_id);

	return texture;
}

ResourceFont *ModuleImporter::LoadFontFile(const char *path)
{
	ResourceFont *font = nullptr;

	if (!App->resources->GetFontByName(App->file_system->GetBaseFileName(path).c_str()))
	{
		font = ResourceFont::ImportFile(path);
	}

	return font;
}

void ModuleImporter::LoadTextureToResource(const char *path, ResourceTexture *texture)
{
	ILuint new_image_id = 0;
	ilGenImages(1, &new_image_id);
	ilBindImage(new_image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilutGLLoadImage((char*)path))
	{
		iluFlipImage();

		texture->id = ilutGLBindTexImage();
		texture->is_custom = true;
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		ilBindImage(0);

		LOG_ENGINE("Texture successfully loaded: %s", path);
	}
	else
	{
		ILenum Error;
		while ((Error = ilGetError()) != IL_NO_ERROR)
		{
			const char* txt = iluErrorString(Error);
			LOG_ENGINE("%d: %s", Error, txt);
		}

		LOG_ENGINE("Error while loading image in %s", path);
		LOG_ENGINE("Error: %s", ilGetString(ilGetError()));
	}

	ilDeleteImages(1, &new_image_id);
}

void ModuleImporter::ApplyTextureToSelectedObject(ResourceTexture *texture)
{
	std::list<GameObject *> selected = App->objects->GetSelectedObjects();
	auto item = selected.begin();
	for (; item != selected.end(); ++item)
	{
		if (*item != nullptr)
		{
			ComponentMaterial *compMaterial = (ComponentMaterial *)(*item)->GetComponent(ComponentType::MATERIAL);

			if ((*item)->HasComponent(ComponentType::MESH) || (*item)->HasComponent(ComponentType::DEFORMABLE_MESH))
			{
				bool exists = true;
				if (compMaterial == nullptr)
				{
					exists = false;
					compMaterial = new ComponentMaterial((*item));
					(*item)->AddComponent(compMaterial);
				}
				compMaterial->SetTexture(texture, TextureType::DIFFUSE);

				if (!exists)
				{
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::ADD_COMPONENT, compMaterial);
				}
				else
				{
					ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, compMaterial);
				}
			}
			else
				LOG_ENGINE("Selected GameObject has no mesh");


			/*if ((*item)->HasComponent(ComponentType::PARTICLES))
			{

				ComponentParticleSystem *particleSystem = (ComponentParticleSystem *)(*item)->GetComponent(ComponentType::PARTICLES);

				if (texture->NeedToLoad())
					texture->LoadMemory();

				particleSystem->SetTexture(texture);
			}
			else
				LOG_ENGINE("Selected GameObject has no particle system");*/
		}
	}
}

ResourceShader *ModuleImporter::LoadShaderFile(const char *path, bool has_been_dropped, bool is_custom)
{
	ResourceShader *shader = nullptr;

	if (!has_been_dropped && !App->file_system->Exists(path))
	{
		return nullptr;
	}

	Resource *shad = nullptr;
	if (App->resources->Exists(path, &shad))
	{

		std::string meta_path_in_assets = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
		u64 ID = App->resources->GetIDFromAlienPath(meta_path_in_assets.data());

		shader = (ResourceShader *)App->resources->GetResourceWithID(ID);

		if (has_been_dropped && !App->objects->GetSelectedObjects().empty())
		{
			ApplyShaderToSelectedObject(shader);
		}
		LOG_ENGINE("This shader was already loaded");

		return shader;
	}
	else
	{
		shader = new ResourceShader(path);

		shader->CreateMetaData();
		App->resources->AddNewFileNode(path, true);

		if (has_been_dropped && !App->objects->GetSelectedObjects().empty())
		{
			ApplyShaderToSelectedObject(shader);
		}
	}

	return shader;
}

void ModuleImporter::ApplyShaderToSelectedObject(ResourceShader *shader)
{
	// TODO
}

void ModuleImporter::ApplyMaterialToSelectedObject(ResourceMaterial* material)
{
	std::list<GameObject*> selected = App->objects->GetSelectedObjects();
	auto item = selected.begin();
	for (; item != selected.end(); ++item)
	{
		if (*item != nullptr)
		{
			if (!(*item)->HasComponent(ComponentType::MESH) && !(*item)->HasComponent(ComponentType::DEFORMABLE_MESH) && !(*item)->HasComponent(ComponentType::PARTICLES))
				continue;	

			ComponentMaterial* materialComp = (ComponentMaterial*)(*item)->GetComponent(ComponentType::MATERIAL);

			if (materialComp == nullptr)
			{
				materialComp = new ComponentMaterial(*item);
				(*item)->AddComponent(materialComp);
			}

			materialComp->SetMaterial(material);
		}
	}
}

void ModuleImporter::ApplyParticleSystemToSelectedObject(std::string path)
{

	std::list<GameObject *> selected = App->objects->GetSelectedObjects();
	auto item = selected.begin();
	for (; item != selected.end(); ++item)
	{
		if (*item != nullptr)
		{

			if (!(*item)->HasComponent(ComponentType::PARTICLES))
				(*item)->AddComponent(new ComponentParticleSystem(*item));

			std::string name = path;
			App->file_system->NormalizePath(name);

			JSON_Value *value = json_parse_file(name.data());
			JSON_Object *object = json_value_get_object(value);

			if (value != nullptr && object != nullptr)
			{
				JSONfilepack *particles = new JSONfilepack(name.data(), object, value);

				JSONArraypack *properties = particles->GetArray("ParticleSystem.Properties");

				if (properties != nullptr)
				{
					ComponentParticleSystem *particleSystem = (ComponentParticleSystem *)(*item)->GetComponent(ComponentType::PARTICLES);
					particleSystem->LoadComponent(properties);
				}

				delete particles;
			}
			else
			{
				LOG_ENGINE("Error loading particle system %s", name.data());
			}
		}
	}
}

void ModuleImporter::LoadParShapesMesh(par_shapes_mesh *shape, ResourceMesh *mesh)
{
	par_shapes_unweld(shape, true);
	par_shapes_compute_normals(shape);

	mesh->num_vertex = shape->npoints;
	mesh->num_index = shape->ntriangles * 3;

	mesh->vertex = new float[mesh->num_vertex * 3];
	mesh->index = new uint[mesh->num_index * 3];

	memcpy(mesh->vertex, shape->points, sizeof(float) * mesh->num_vertex * 3);
	memcpy(mesh->index, shape->triangles, sizeof(PAR_SHAPES_T) * mesh->num_index);

	if (shape->tcoords != nullptr)
	{
		mesh->uv_cords = new float[mesh->num_vertex * 3];
		memcpy(mesh->uv_cords, shape->tcoords, sizeof(float) * mesh->num_vertex * 3);
	}

	if (shape->normals != nullptr)
	{
		mesh->normals = new float[mesh->num_vertex * 3];

		memcpy(mesh->normals, shape->normals, sizeof(float) * mesh->num_vertex * 3);

		mesh->center_point_normal = new float[shape->ntriangles * 3];
		mesh->center_point = new float[shape->ntriangles * 3];
		mesh->num_faces = shape->ntriangles;
		for (uint i = 0; i < mesh->num_index; i += 3)
		{
			uint index1 = mesh->index[i] * 3;
			uint index2 = mesh->index[i + 1] * 3;
			uint index3 = mesh->index[i + 2] * 3;

			float3 x0(mesh->vertex[index1], mesh->vertex[index1 + 1], mesh->vertex[index1 + 2]);
			float3 x1(mesh->vertex[index2], mesh->vertex[index2 + 1], mesh->vertex[index2 + 2]);
			float3 x2(mesh->vertex[index3], mesh->vertex[index3 + 1], mesh->vertex[index3 + 2]);

			float3 v0 = x0 - x2;
			float3 v1 = x1 - x2;
			float3 n = v0.Cross(v1);

			float3 normalized = n.Normalized();

			mesh->center_point[i] = (x0.x + x1.x + x2.x) / 3;
			mesh->center_point[i + 1] = (x0.y + x1.y + x2.y) / 3;
			mesh->center_point[i + 2] = (x0.z + x1.z + x2.z) / 3;

			mesh->center_point_normal[i] = normalized.x;
			mesh->center_point_normal[i + 1] = normalized.y;
			mesh->center_point_normal[i + 2] = normalized.z;
		}
	}
	mesh->InitBuffers();
}

ResourceMesh *ModuleImporter::LoadEngineModels(const char *path)
{
	ResourceMesh *ret = nullptr;

	const aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
												  aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);

	ret = new ResourceMesh();

	// get vertex
	ret->vertex = new float[scene->mMeshes[0]->mNumVertices * 3];
	memcpy(ret->vertex, scene->mMeshes[0]->mVertices, sizeof(float) * scene->mMeshes[0]->mNumVertices * 3);
	ret->num_vertex = scene->mMeshes[0]->mNumVertices;

	// get index
	if (scene->mMeshes[0]->HasFaces())
	{
		ret->num_index = scene->mMeshes[0]->mNumFaces * 3;
		ret->index = new uint[ret->num_index]; // assume each face is a triangle
		for (uint i = 0; i < scene->mMeshes[0]->mNumFaces; ++i)
		{
			if (scene->mMeshes[0]->mFaces[i].mNumIndices != 3)
			{
				uint non[3] = {0, 0, 0};
				memcpy(&ret->index[i * 3], non, 3 * sizeof(uint));
				LOG_ENGINE("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&ret->index[i * 3], scene->mMeshes[0]->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}
	// get normals
	if (scene->mMeshes[0]->HasNormals())
	{
		ret->normals = new float[scene->mMeshes[0]->mNumVertices * 3];
		memcpy(ret->normals, scene->mMeshes[0]->mNormals, sizeof(float) * scene->mMeshes[0]->mNumVertices * 3);

		ret->center_point_normal = new float[scene->mMeshes[0]->mNumFaces * 3];
		ret->center_point = new float[scene->mMeshes[0]->mNumFaces * 3];
		ret->num_faces = scene->mMeshes[0]->mNumFaces;
		for (uint i = 0; i < ret->num_index; i += 3)
		{
			uint index1 = ret->index[i] * 3;
			uint index2 = ret->index[i + 1] * 3;
			uint index3 = ret->index[i + 2] * 3;

			float3 x0(ret->vertex[index1], ret->vertex[index1 + 1], ret->vertex[index1 + 2]);
			float3 x1(ret->vertex[index2], ret->vertex[index2 + 1], ret->vertex[index2 + 2]);
			float3 x2(ret->vertex[index3], ret->vertex[index3 + 1], ret->vertex[index3 + 2]);

			float3 v0 = x0 - x2;
			float3 v1 = x1 - x2;
			float3 n = v0.Cross(v1);

			float3 normalized = n.Normalized();

			ret->center_point[i] = (x0.x + x1.x + x2.x) / 3;
			ret->center_point[i + 1] = (x0.y + x1.y + x2.y) / 3;
			ret->center_point[i + 2] = (x0.z + x1.z + x2.z) / 3;

			ret->center_point_normal[i] = normalized.x;
			ret->center_point_normal[i + 1] = normalized.y;
			ret->center_point_normal[i + 2] = normalized.z;
		}
	}
	// get UV
	if (scene->mMeshes[0]->HasTextureCoords(0))
	{
		ret->uv_cords = new float[scene->mMeshes[0]->mNumVertices * 3];
		memcpy(ret->uv_cords, (float *)scene->mMeshes[0]->mTextureCoords[0], sizeof(float) * scene->mMeshes[0]->mNumVertices * 3);
	}

	ret->InitBuffers();

	aiReleaseImport(scene);
	if (ret != nullptr)
	{
		ret->is_custom = false;
	}
	return ret;
}

bool ModuleImporter::ReImportModel(ResourceModel *model)
{
	bool ret = true;

	const aiScene *scene = aiImportFile(model->GetAssetsPath(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
																	aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);

	if (scene != nullptr)
	{
		model->name = App->file_system->GetBaseFileName(model->GetAssetsPath());
		this->model = model;

		//Import meshes & bones
		if (scene->HasMeshes())
		{
			for (int i = 0; i < scene->mNumMeshes; ++i)
			{
				//Import mesh here.
				LoadMesh(scene->mMeshes[i]);

				//Import bones of mesh
				if (scene->mMeshes[i]->HasBones())
				{
					model->meshes_attached[i]->deformable = true;
					for (int j = 0; j < scene->mMeshes[i]->mNumBones; ++j)
					{
						LoadBone(scene->mMeshes[i]->mBones[j], model->meshes_attached[i]->name);
					}
				}
			}
		}
		ReImportAnimations(model, scene);

		// start recursive function to all nodes
		for (uint i = 0; i < scene->mRootNode->mNumChildren; ++i)
		{
			LoadNode(scene->mRootNode->mChildren[i], scene, 1);
		}

		// create the meta data files like .alien
		if (model->CreateMetaData(model->ID))
		{
			App->resources->AddResource(model);
			model->FreeMemory();
		}

		this->model = nullptr;
	}
	else
	{
		ret = false;
		LOG_ENGINE("Error loading model %s", model->GetAssetsPath());
		LOG_ENGINE("Error type: %s", aiGetErrorString());
	}
	aiReleaseImport(scene);

	return ret;
}

void ModuleImporter::ReImportAnimations(ResourceModel *model, const aiScene *scene)
{
	if (scene->HasAnimations())
	{
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			LoadAnimation(scene->mAnimations[i]);
		}

		JSON_Value *value = json_parse_file(model->meta_data_path.data());
		JSON_Object *object = json_value_get_object(value);

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack *meta = new JSONfilepack(model->meta_data_path.data(), object, value);
			uint num_anims = meta->GetNumber("Meta.NumAnimations");
			JSONArraypack *anims_meta = meta->GetArray("Meta.Animations");

			for (int i = 0; i < num_anims; ++i)
			{
				if (i > 0)
				{
					ResourceAnimation *new_anim = new ResourceAnimation();
					model->animations_attached.push_back(new_anim);
					App->resources->AddResource(new_anim);
					model->animations_attached[i]->Copy(model->animations_attached[0]);
				}
				anims_meta->GetNode(i);
				model->animations_attached[i]->name = anims_meta->GetString("Name");
				model->animations_attached[i]->loops = anims_meta->GetBoolean("Loops");
				model->animations_attached[i]->start_tick = anims_meta->GetNumber("Start_Tick");
				model->animations_attached[i]->end_tick = anims_meta->GetNumber("End_Tick");
			}
			delete meta;
		}
	}
}
