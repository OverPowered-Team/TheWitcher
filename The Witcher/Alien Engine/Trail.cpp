#include "Application.h"
#include "Trail.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "ComponentTrail.h"
#include "ModuleResources.h"
#include "imgui/imgui_internal.h"
#include "ResourceShader.h"


Trail::Trail()
{
}

Trail::Trail(ComponentTrail* comp, GameObject* owner) : owner(owner), trail_comp(comp)
{
	default_material = new ResourceMaterial();
	default_material->SetName("Trail Material");
	
	SetMaterial(default_material);
	material->SetShader(App->resources->trail_shader);

	originalSpawnBox = trail_comp->GetOBBFromObject();
	if (!originalSpawnBox.IsFinite())
	{
		originalSpawnBox = math::AABB::FromCenterAndSize(math::float3::zero(), math::float3::one());
	}
	spawnBox = _spawnBox = originalSpawnBox;

	float vertex[] =
	{
		// 0
		-0.5f, 0.5f, 0.f,
		// 1 
		0.5f, 0.5f, 0.f,
		// 2 
		-0.5f, -0.5f, 0.f,
		// 3 
		0.5f, -0.5f, 0.f,
	};

	float uv[] =
	{
		// 0
		0.0f, 1.0f,
		// 1
		1.0f, 1.0f,
		// 2
		0.0f, 0.0f,
		// 3
		1.0f, 0.0f,
	};

	uint index[] =
	{
		// First tri
		2, 1, 0,
		// Second tri
		2, 3, 1
	};


	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VERTEX BUFFER
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(0);

	// UV BUFFER
	glGenBuffers(1, &id_uv);
	glBindBuffer(GL_ARRAY_BUFFER, id_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// INDEX BUFFER
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}

Trail::~Trail()
{

	for (std::list< TrailNode*>::iterator iter = trailVertex.begin(); iter != trailVertex.end(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}
	trailVertex.clear();

	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_uv);


	if (material != nullptr) {

		material->DecreaseReferences();
		material = nullptr;
	}

	if (default_material) {
		default_material->DecreaseReferences();
		delete default_material;
		default_material = nullptr;
	}

	 owner = nullptr;
	 trail_comp = nullptr;

}

bool Trail::PreUpdate(float dt)
{
	//if (trailVertex.size() > 0)
	//{
	//	// Remove old nodes

	//	for (std::list< TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
	//	{
	//		(*curr)->current_lifeTime += dt;

	//		if ((*curr)->current_lifeTime > lifeTime)
	//		{
	//			(*curr)->to_delete = true;
	//		}
	//	}

	//}
	
	return true;
}

bool Trail::Update(float dt)
{
	if (emitting)
	{
		// Get the new trail vertex
		math::float3 originHigh = float3::zero();
		math::float3 originLow = float3::zero();

		if (customSpawn)
		{
			
			math::float4x4 transformMatrix = owner->transform->GetGlobalMatrix();
			spawnBox = originalSpawnBox;
			spawnBox.Transform(transformMatrix);

			originHigh = spawnBox.FaceCenterPoint(high);
			originLow = spawnBox.FaceCenterPoint(low);
		}
		else
		{
			originHigh = trail_comp->GetOBBFromObject().FaceCenterPoint(high);
			originLow = trail_comp->GetOBBFromObject().FaceCenterPoint(low);
		}

		// Check we already have a trail
		if (trailVertex.size() > 1)
		{
			TrailNode* last = trailVertex.back();

			// If the distance between the last vertex and the new vertex is
			// greater than the minimum distance, we add a new node to the trail
			if (last && originHigh.Distance(last->originHigh) > minDistance)
			{
				TrailNode* node = new TrailNode(originHigh, originLow);

				trailVertex.push_back(node);
			}

			// Otherwise we modify the last trail node
			else
			{
				last->originHigh = originHigh;
				last->originLow = originLow;

				//last->createTime = SDL_GetTicks();
			}
		}

		// If we do not have a trail, we add the node directly
		else
		{
			TrailNode* node = new TrailNode(originHigh, originLow);

			trailVertex.push_back(node);
			current_time = 0;
		}

		//timer.Start();
	}

	if (trailVertex.size() > 0)
	{
		// Remove old nodes
		if (current_time >= time)
		{
			//Uint32 time = SDL_GetTicks();
			std::list< TrailNode*> toRemove;
			for (std::list< TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
			{
				(*curr)->current_lifeTime += dt;

				if ((*curr)->current_lifeTime > lifeTime)
				{
					toRemove.push_back(*curr);
				}
				else break;
			}

			for (std::list< TrailNode*>::iterator it = toRemove.begin(); it != toRemove.end(); ++it)
			{
				trailVertex.remove(*it);
				delete* it;

				
					
			}
		}
		else
			current_time += dt;

	}
	

	

	
	return true;
}

bool Trail::PostUpdate(float dt)
{
	//if (trailVertex.size() > 0)
	//{
	//	// Remove old nodes

	//	for (std::list< TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
	//	{
	//		
	//		if ((*curr)->to_delete)
	//		{
	//			/*trailVertex.remove(*curr);
	//			delete (*curr);
	//			(*curr) = nullptr;*/

	//			delete (*iter);
	//			(*iter) = nullptr;
	//			iter = particles.erase(iter);
	//		}
	//		else
	//			break;
	//	}

	//}
	


	return true;
}

bool Trail::Draw()
{
	
	if (!trailVertex.empty())
	{
		float i = 0.0f;
		float size = trailVertex.size() + 1;

		for (std::list<TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
		{
			i++;
			std::list<TrailNode*>::iterator next = curr;
			++next;
			if (next != trailVertex.end())
			{

				// Bind Trail shader

				// Bind Material texture
				material->ApplyMaterial();
				
				// Update Uniforms
				float4x4 model_matrix = float4x4::identity();

				float currUV = (float(i) / size);
				float nextUV = (float(i + 1) / size);
				math::float3 originHigh = (*curr)->originHigh;
				math::float3 originLow = (*curr)->originLow;
				math::float3 destinationHigh = (*next)->originHigh;
				math::float3 destinationLow = (*next)->originLow;

				if (orient)
					RearrangeVertex(this, curr, next, currUV, nextUV, originHigh, originLow, destinationHigh, destinationLow);



				UpdateUniforms(material, model_matrix, currUV, nextUV, originHigh, originLow, destinationHigh, destinationLow);

				// Draw buffers
				glBindVertexArray(vao);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

				//Disable stuff
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				material->UnbindMaterial();
			}
		}

	}
	
	
	
	
	
	return true;


}



void Trail::SetMaterial(ResourceMaterial* mat)
{

	if (mat == nullptr)
		return;

	if (material != nullptr)
	{
		material->DecreaseReferences();
		//material = nullptr;
	}

	material = mat;
	material->IncreaseReferences();
}

void Trail::RemoveMaterial()
{
	SetMaterial(default_material);
}

void Trail::UpdateUniforms(ResourceMaterial* resource_material, float4x4 globalMatrix, float currUV, float nextUV, float3 originHigh, float3 originLow, float3 destHigh, float3 destLow)
{
	resource_material->used_shader->SetUniformMat4f("model", globalMatrix.Transposed());
	
	resource_material->used_shader->SetUniform1f("currUV", currUV);
	resource_material->used_shader->SetUniform1f("nextUV", nextUV);

	resource_material->used_shader->SetUniformFloat3("vertex1", originHigh);
	resource_material->used_shader->SetUniformFloat3("vertex2", originLow);
	resource_material->used_shader->SetUniformFloat3("vertex3", destHigh);
	resource_material->used_shader->SetUniformFloat3("vertex4", destLow);
}


void Trail::RearrangeVertex(Trail* trail, std::list<TrailNode*>::iterator& curr, std::list<TrailNode*>::iterator& next, float& currUV, float& nextUV, math::float3& originHigh, math::float3& originLow, math::float3& destinationHigh, math::float3& destinationLow)
{
	// Rearrange vertex
	float origin = 0;
	float dest = 0;

	GetOriginAndDest(trail, origin, curr, dest, next);

	if (origin < dest)
	{
		float tmp = currUV;
		currUV = nextUV;
		nextUV = tmp;

		math::float3 tmph = originHigh;
		math::float3 tmpl = originLow;

		originHigh = destinationHigh;
		originLow = destinationLow;

		destinationHigh = tmph;
		destinationLow = tmpl;
	}
}

void Trail::GetOriginAndDest(Trail* trail, float& origin, std::list<TrailNode*>::iterator& curr, float& dest, std::list<TrailNode*>::iterator& next)
{

	switch (trail->vector)
	{
	case TrailVector::X:
		origin = (*curr)->originHigh.x;
		dest = (*next)->originHigh.x;
		break;
	case TrailVector::Y:
		// This is not right
		origin = (*curr)->originHigh.x;
		dest = (*next)->originHigh.x;
		break;
	case TrailVector::Z:
		dest = (*curr)->originHigh.z;
		origin = (*next)->originHigh.z;
		break;
	default:
		break;
	}


}




void Trail::Start()
{
	emitting = true;
}

void Trail::Stop()
{
	emitting = false;
}

bool Trail::isPlaying() const
{
	return emitting;
}

void Trail::SetVector(TrailVector vec)
{
	switch (vec)
	{
	case TrailVector::X:
		low = 4; high = 5;
		break;
	case TrailVector::Y:
		low = 2; high = 3;
		break;
	case TrailVector::Z:
		low = 0; high = 1;
		break;
	default:
		break;
	}

	vector = vec;
}

void Trail::SetTime(int time)
{
	this->time = time;
}

void Trail::SetLifeTime(int lifeTime)
{
	this->lifeTime = lifeTime;
}

void Trail::SetMinDistance(float minDistance)
{
	this->minDistance = minDistance;
}

void Trail::SetSpawnSize(math::float3 size)
{
	_spawnBox = originalSpawnBox = math::AABB::FromCenterAndSize(math::float3::zero(), size).ToOBB();
}

void Trail::SetColor(math::float4 color)
{
	this->color = color;
}
