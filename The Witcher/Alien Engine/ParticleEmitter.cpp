#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "RandomHelper.h"

#include "MathGeoLib/include/Geometry/Circle.h"
#include "GL/gl.h"

ParticleEmmitter::ParticleEmmitter() : particleSystem()
{
}

ParticleEmmitter::~ParticleEmmitter()
{
}

void ParticleEmmitter::Update(float dt)
{
	lifeTime += dt;
	current_delay += dt;

	if (current_delay >= delay)
		currentSpawnTime += dt;
		

	if (loop && lifeTime >= maxLifeTime)
		Reset();
}

int ParticleEmmitter::GetParticlesToInstantiate()
{
	if (spawnRate <= 0)
		return 0;

	// How many particles should instantiate this frame
	float particlesToSpawn = currentSpawnTime / spawnRate;

	// Round low to Int -> Example 2,7(float) -> 2(int) 
	int particlesToInstantiate = (int)particlesToSpawn;

	// Since we are losing some info in the Rounding, we need to calculate the time we lost:
	// We calculate the time we did not take into account (in the example the 0.7 particle) and calculate
	// How much time it should take to instantiate that 0.7 particle. We start from there in the next frame
	currentSpawnTime = (particlesToSpawn - particlesToInstantiate) * spawnRate;


	// Return the number of particles to instantiate this frame
	return particlesToInstantiate;
}

int ParticleEmmitter::GetParticlesToBurst()
{
	int particlesToInstantiate = 0;

	for (int i = 0; i < bursts.size(); ++i)
	{
		if (bursts[i].hasBursted)
			continue;

		if (lifeTime >= bursts[i].timeToBurst)
		{
			bursts[i].hasBursted = true;
			particlesToInstantiate += bursts[i].partsToInstantiate;
		}

	}

	return particlesToInstantiate;
}

void ParticleEmmitter::DebugDrawEmmitter()
{
	switch (shape)
	{
	case Emmitter_Shape::Sphere: DrawSphere(radius, 10, 10); break;
	case Emmitter_Shape::Hemisphere: break;
	case Emmitter_Shape::Cube: DrawCube(cubeSize.x, cubeSize.y, cubeSize.z); break;
	case Emmitter_Shape::Cone: DrawCone(); break;
	default: break;
	}
}

void ParticleEmmitter::GetInitialValues(float3& position, float3& velocity, float speed, bool globalTransform)
{
	math::LCG lcg; 	// Maybe here we could use some pcg ol' tricks of yours

	// Set the spawn position
	// If its not on local space (global space) instantiate it on the emitter position
	// If its in local space, then instantiate it in pos 0,0,0
	float3 emmitterCenterPosition = float3::zero();
	if (globalTransform)
		emmitterCenterPosition = GetWorldPosition();

	switch (shape)
	{
	case Emmitter_Shape::Sphere:

		switch (zone)
		{
		case Emmitter_Zone::Base:
			position = emmitterCenterPosition;
			velocity = float3::RandomDir(lcg, 1.0f) * speed;
			break;

		case Emmitter_Zone::Volume:
			// Gets a random position inside the sphere
			position = emmitterCenterPosition + float3::RandomDir(lcg, 1.0f) * Random::GetRandomFloatBetweenTwo(0, radius);
			// Calculates the direction respect the center of the sphere
			velocity = (position - emmitterCenterPosition).Normalized() * speed;
			break;
		}

		break;

	case Emmitter_Shape::Hemisphere:

		break;

	case Emmitter_Shape::Cube:
	{
		// Get a random position inside the cube (cube centered in emmitter pos), without any rotation
		position = emmitterCenterPosition;
		position.x += Random::GetRandomFloatBetweenTwo(-cubeSize.x * 0.5f, cubeSize.x * 0.5f);
		position.y += Random::GetRandomFloatBetweenTwo(-cubeSize.y * 0.5f, cubeSize.y * 0.5f);
		position.z += Random::GetRandomFloatBetweenTwo(-cubeSize.z * 0.5f, cubeSize.z * 0.5f);

		// Get the vector between the position of the emmitter and the random position gotten
		float3 relativeRandomPosition = (position - emmitterCenterPosition);

		// Apply the rotation of the emmitter to that vector 
		relativeRandomPosition = relativeRandomPosition * GetWorldRotation().ToFloat3x3().Transposed();

		// Add that vector to the emmitter position 
		position = emmitterCenterPosition + relativeRandomPosition;

		velocity = GetWorldRotation().WorldZ() * speed;
	}
	break;

	case Emmitter_Shape::Cone:
	{
		math::Circle baseCircle = math::Circle(emmitterCenterPosition, GetWorldRotation().WorldZ(), radius);
		math::Circle outerCircle = math::Circle(emmitterCenterPosition, GetWorldRotation().WorldZ(), outRadius);
		// (baseCircle.RandomPointInside(lcg) -> Gets a random point in the surface of the circle 
		//position = (baseCircle.RandomPointInside(lcg) - GetWorldPosition()).Normalized() * pcg32_boundedrand_r(&rng_bounded, (radius - 0) + 1);


		// For the moment lets emmit from the center point of the emmitter
		position = emmitterCenterPosition;

		// Get direction respect the second circle of the cone
		float3 circleRandomPoint = (outerCircle.RandomPointInside(lcg) - emmitterCenterPosition).Normalized() * Random::GetRandomFloatBetweenTwo(0, outRadius);
		circleRandomPoint += emmitterCenterPosition; // Move it to the emmitter location, otherwise its on the Origin.
		circleRandomPoint += GetWorldRotation().WorldZ() * distance; // Move the point forward
		velocity = (circleRandomPoint - position).Normalized() * speed;  // Finally make the initial velocity vector
	}
	break;

	default:
		break;
	}
}

bool ParticleEmmitter::HasBurstsActive() const
{
	if (bursts.empty())
		return false;

	for (int i = 0; i < bursts.size(); ++i)
	{
		if (!bursts[i].hasBursted)
			return true;
	}

	return false;
}

void ParticleEmmitter::RemoveBurst(int index)
{
	if (bursts.empty())
		return;

	if (index > bursts.size() || index < 0)
		return;

	bursts.erase(bursts.begin() + index);
}

void ParticleEmmitter::DrawSphere(double r, int lats, int longs)
{
	for (int i = 0; i <= lats; i++) {
		double lat0 = math::pi * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = math::pi * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glPushMatrix();
		glMultMatrixf((GLfloat*)&(float4x4::FromTRS(GetWorldPosition(), GetWorldRotation(), float3(1.f, 1.f, 1.f)).Transposed()));
		glColor3f(0.f, 1.f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= longs; j++) {
			double lng = 2 * math::pi * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(r * x * zr0, r * y * zr0, r * z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(r * x * zr1, r * y * zr1, r * z1);
		}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1.f, 1.f, 1.f);
		glPopMatrix();
	}
}

void ParticleEmmitter::DrawCube(float length, float height, float width)
{
	length *= 0.5f;
	height *= 0.5f;
	width *= 0.5f;

	glPushMatrix();
	glMultMatrixf((GLfloat*)&(float4x4::FromTRS(GetWorldPosition(), GetWorldRotation(), float3(1.f, 1.f, 1.f)).Transposed()));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-length, -height, width);
	glVertex3f(length, -height, width);
	glVertex3f(length, height, width);
	glVertex3f(-length, height, width);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(length, -height, -width);
	glVertex3f(-length, -height, -width);
	glVertex3f(-length, height, -width);
	glVertex3f(length, height, -width);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(length, -height, width);
	glVertex3f(length, -height, -width);
	glVertex3f(length, height, -width);
	glVertex3f(length, height, width);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-length, -height, -width);
	glVertex3f(-length, -height, width);
	glVertex3f(-length, height, width);
	glVertex3f(-length, height, -width);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-length, height, width);
	glVertex3f(length, height, width);
	glVertex3f(length, height, -width);
	glVertex3f(-length, height, -width);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-length, -height, -width);
	glVertex3f(length, -height, -width);
	glVertex3f(length, -height, width);
	glVertex3f(-length, -height, width);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

void ParticleEmmitter::DrawCone()
{
	int n = 30;

	//float innerRadius = radius; 
	float innerRadius = 0; // For the moment, the inner circle is always radius = 0

	glPushMatrix();
	glMultMatrixf((GLfloat*)&(float4x4::FromTRS(GetWorldPosition(), GetWorldRotation(), float3(1.f, 1.f, 1.f)).Transposed()));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_POLYGON);

	//// Cylinder Bottom
	for (int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * math::pi / 180; // degrees to radians
		glVertex3f(innerRadius * cos(a), innerRadius * sin(a), 0);
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * math::pi / 180; // degrees to radians
		glVertex3f(outRadius * cos(a), outRadius * sin(a), distance);
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i < 480; i += (360 / n))
	{
		float a = i * math::pi / 180; // degrees to radians

		glVertex3f(outRadius * cos(a), outRadius * sin(a), distance);
		glVertex3f(innerRadius * cos(a), innerRadius * sin(a), 0.0f);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	// Draw Spawn Points 
	/*glColor3f(0.f, 1.f, 1.f);
	glBegin(GL_POINTS);

	for (int i = 0; i < debug.size(); ++i)
		glVertex3f(debug[i].x, debug[i].y, debug[i].z);

	glEnd();
	glColor3f(1.f, 1.f, 1.f);*/

}

void ParticleEmmitter::Reset()
{
	lifeTime = 0.f;
	playing = true;
	ResetBursts();
}

void ParticleEmmitter::ResetBursts()
{
	for (int i = 0; i < bursts.size(); ++i)
	{
		bursts[i].hasBursted = false;
	}
}

bool ParticleEmmitter::isActive() const
{
	return (lifeTime < maxLifeTime) && playing == true;
}

bool ParticleEmmitter::isDelayed() const
{
	return current_delay < delay;
}


// ----------------- Modify Emmitter -----------------

void ParticleEmmitter::SetShape(Emmitter_Shape shape)
{
	this->shape = shape;
}

void ParticleEmmitter::SetZone(Emmitter_Zone zone)
{
	this->zone = zone;
}

void ParticleEmmitter::SetRadius(float radius)
{
	this->radius = radius;
}

void ParticleEmmitter::SetOutRadius(float radius)
{
	this->outRadius = radius;
}

void ParticleEmmitter::SetDistance(float distance)
{
	this->distance = distance;
}

void ParticleEmmitter::SetMaxLife(float maxLife)
{
	this->maxLifeTime = maxLife;

	for (int i = 0; i < bursts.size(); ++i)
	{
		if (bursts[i].timeToBurst > maxLifeTime)
			bursts[i].timeToBurst = maxLifeTime;
	}
}

void ParticleEmmitter::SetCurrentLife(float currentLife)
{
	this->lifeTime = currentLife;
}

void ParticleEmmitter::SetSpawnRate(float spawnRate)
{
	if (spawnRate == 0)
		this->spawnRate = 0.f;
	else
		this->spawnRate = 1.f / spawnRate;
}

void ParticleEmmitter::SetDelay(float delay)
{
	this->delay = delay;
}

void ParticleEmmitter::SetLoop(bool isLoop)
{
	loop = isLoop;
}

void ParticleEmmitter::SetPosition(float3 position)
{
	this->position = position;
}

void ParticleEmmitter::SetRelativePosition(float3 position)
{
	relativePosition = position;
}


void ParticleEmmitter::SetRotation(Quat rotation)
{
	this->rotation = rotation;
}

void ParticleEmmitter::SetRotation(float3 rotation)
{
	this->rotation = Quat::FromEulerXYZ(math::DegToRad(rotation.x), math::DegToRad(rotation.y), math::DegToRad(rotation.z));
}

void ParticleEmmitter::SetRelativeRotation(float3 rotation)
{
	this->relativeRotation = Quat::FromEulerXYZ(math::DegToRad(rotation.x), math::DegToRad(rotation.y), math::DegToRad(rotation.z));
}

void ParticleEmmitter::SetScale(float3 scale)
{
	this->scale = scale;
}

// --------------------------------------------------


Emmitter_Shape ParticleEmmitter::GetShape() const
{
	return shape;
}

Emmitter_Zone ParticleEmmitter::GetZone() const
{
	return zone;
}

float ParticleEmmitter::GetRadius() const
{
	return radius;
}

float ParticleEmmitter::GetOutRadius() const
{
	return outRadius;
}

float ParticleEmmitter::GetDistance() const
{
	return distance;
}

float* ParticleEmmitter::GetCubeSize() const
{
	return (float*)&cubeSize;
}

float ParticleEmmitter::GetMaxLife() const
{
	return maxLifeTime;
}

float ParticleEmmitter::GetCurrentLife() const
{
	return lifeTime;
}

float ParticleEmmitter::GetSpawnRate() const
{
	if (spawnRate == 0)
		return 0.f;
	else
		return 1.f / spawnRate;
}

float ParticleEmmitter::GetDelay() const
{
	return delay;
}

bool ParticleEmmitter::GetLoop() const
{
	return loop;
}

float3 ParticleEmmitter::GetPosition() const
{
	return position;
}

float3 ParticleEmmitter::GetRelativePosition() const
{
	return relativePosition;
}

float3 ParticleEmmitter::GetWorldPosition() const
{
	return position + relativePosition;
}

float3 ParticleEmmitter::GetRotation() const
{
	return math::RadToDeg(rotation.ToEulerXYZ());
}

float3 ParticleEmmitter::GetRelativeRotation() const
{
	return math::RadToDeg(relativeRotation.ToEulerXYZ());
}

Quat ParticleEmmitter::GetWorldRotation() const
{
	return rotation * relativeRotation;
}

float3 ParticleEmmitter::GetScale() const
{
	return scale;
}

float4x4 ParticleEmmitter::GetGlobalTransform() const
{
	return float4x4::FromTRS(position, rotation, float3::one());
}

void ParticleEmmitter::Stop()
{
	playing = false;
}

void ParticleEmmitter::Play()
{
	playing = true;
}

void ParticleEmmitter::SetCubeSize(float3 size)
{
	this->cubeSize = size;
}