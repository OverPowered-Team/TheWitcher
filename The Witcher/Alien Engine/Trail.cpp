#include "Trail.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "ComponentTrail.h"
Trail::Trail()
{
}

Trail::Trail(ComponentTrail* comp, GameObject* owner) : owner(owner), trail_comp(comp)
{
}

Trail::~Trail()
{
	for (std::list< TrailNode*>::iterator iter = trailVertex.begin(); iter != trailVertex.end(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}
}

bool Trail::PreUpdate(float dt)
{
	if (trailVertex.size() > 0)
	{
		// Remove old nodes

		for (std::list< TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
		{
			(*curr)->current_lifeTime += dt;

			if ((*curr)->current_lifeTime > lifeTime)
			{
				(*curr)->to_delete = true;
			}
		}

	}
	return true;
}

bool Trail::Update(float dt)
{
	if (create)
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
		}

		//timer.Start();
	}
	return true;
}

bool Trail::PostUpdate(float dt)
{
	if (trailVertex.size() > 0)
	{
		// Remove old nodes

		for (std::list< TrailNode*>::iterator curr = trailVertex.begin(); curr != trailVertex.end(); ++curr)
		{
			
			if ((*curr)->to_delete)
			{
				trailVertex.remove(*curr);
				delete* curr;
				(*curr) = nullptr;
			}
		}

	}



	return true;
}

bool Trail::Draw()
{
	return false;
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
	create = true;
}

void Trail::Stop()
{
	create = false;
}