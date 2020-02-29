#include "ParticleSystem.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "Camera.h"

#include "GL/gl.h"

ParticleSystem::ParticleSystem()
{
	particles.reserve(MAX_PARTICLES);
	emmitter.particleSystem = this;

	float planeVertex[] =
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

	float planeUVCoords[] =
	{
		// 0
		0.0f, 0.0f, 
		// 1
		1.0f, 0.0f,
		// 2
		0.0f, 1.0f,
		// 3
		1.0f, 1.0f,
	};

	uint planeIndex[] =
	{
		// First tri
		2, 1, 0,
		// Second tri
		2, 3, 1
	};

	glGenBuffers(1, &planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, planeVertex, GL_STATIC_DRAW);

	// index
	glGenBuffers(1, &planeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, planeIndex, GL_STATIC_DRAW);

	// UV
	glGenBuffers(1, &planeUVsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, planeUVsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, planeUVCoords, GL_STATIC_DRAW);
	
}

ParticleSystem::~ParticleSystem()
{
	// Delete all particles
	for (std::vector<Particle*>::iterator iter = particles.begin(); iter != particles.end(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}

	particles.clear();
}

bool ParticleSystem::PreUpdate(float dt)
{
	// Spawn option from particle system 

	for (uint i = 0; i < particles.size(); ++i)
	{
		particles[i]->PreUpdate(dt);
	}

	return true;
}

bool ParticleSystem::Update(float dt)
{
	// --------------- Emmitter -----------------

	if (emmitter.isActive()) // If its active, update it, and check if it has to spawn
	{
		emmitter.Update(dt);

		int particlesToCreate = emmitter.GetParticlesToInstantiate();
		InstantiateParticles(particlesToCreate);

		if (emmitter.HasBurstsActive())
		{
			int particlesToBurst = emmitter.GetParticlesToBurst();
			InstantiateParticles(particlesToBurst);
		}

	}

	// ------------------------------------------


	// --------------- Update Particles -----------------
	for (uint i = 0; i < particles.size(); ++i)
	{
		particles[i]->Update(dt);
	}


	return true;
}

bool ParticleSystem::PostUpdate(float dt)
{
	for (std::vector<Particle*>::iterator iter = particles.begin(); iter != particles.end();)
	{
		(*iter)->PostUpdate(dt);

		// Delete the particles that are needed
		if ((*iter)->to_delete)
		{
			delete (*iter);
			(*iter) = nullptr;
			iter = particles.erase(iter);
			totalParticles--;
		}
		else
			iter++;
	}

	// ------------------ ORDER PARTICLES NEAR TO FAR ------------------------

	if(App->renderer3D->GetCurrentMainCamera() != nullptr) // In case we are in game mode and no camera exists
		std::sort(particles.begin(), particles.end(), compareParticles);

	return true;
}

void ParticleSystem::CreateParticle(ParticleInfo info, ParticleMutableInfo endInfo)
{
	if (totalParticles > MAX_PARTICLES)
		return;

	particles.push_back(new Particle(this, info, endInfo));
	totalParticles++;
}

void ParticleSystem::InstantiateParticles(int particles)
{
	for (uint i = 0; i < particles; ++i)
	{
		emmitter.GetInitialValues(particleInfo.position, particleInfo.velocity, particleInfo.speed, particleInfo.globalTransform);
		CreateParticle(particleInfo, endInfo);
	}
}

void ParticleSystem::DrawParticles()
{
	// Debugging drawing points in particles Position
	//DrawPointsForParticles();

	ComponentCamera* mainCamera = App->renderer3D->GetCurrentMainCamera();
	//-------------------------- DRAW PARTICLES FAR TO NEAR ------------------

	for (std::vector<Particle*>::reverse_iterator iter = particles.rbegin(); iter != particles.rend(); ++iter)
	{
		(*iter)->Orientate(mainCamera);
		(*iter)->Rotate();
		(*iter)->Draw();
	}

}

void ParticleSystem::DrawPointsForParticles()
{
	glBegin(GL_POINTS);
	glColor3f(0.f, 0.f, 1.f);

	for (std::vector<Particle*>::iterator iter = particles.begin(); iter != particles.end(); ++iter)
	{

		// Draw Particle
		float3 particlePos = (*iter)->GetPosition();
		glVertex3f(particlePos.x, particlePos.y, particlePos.z);

		// Render light if any
		//(*iter)->light.Render(); // Don't know how we'll handle this in mesh mode 

	}

	glColor3f(1.f, 1.f, 1.f);
	glEnd();
}

void ParticleSystem::DrawEmmitter()
{
	emmitter.DebugDrawEmmitter();
}

void ParticleSystem::SetBillboardType(BillboardType type)
{
	bbType = type;
}

BillboardType ParticleSystem::GetBillboardType() const
{
	return bbType;
}

uint ParticleSystem::GetTotalParticles() const
{
	return totalParticles;
}

bool ParticleSystem::isSystemActive() const
{
	return !particles.empty() || emmitter.isActive();
}

bool ParticleSystem::isPlaying() const
{
	return playing;
}

void ParticleSystem::Play()
{
	playing = true; 
}

void ParticleSystem::Pause()
{
	playing = false;
}

void ParticleSystem::Restart()
{
	ResetSystem();
	Play();
}

void ParticleSystem::ResetSystem()
{
	// Reset Emmitter
	emmitter.Reset();

	// Delete all particles
	for (std::vector<Particle*>::iterator iter = particles.begin(); iter != particles.end(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}

	particles.clear();

	// Update counter
	totalParticles = 0u;
}

bool compareParticles(Particle* a, Particle* b)
{
	return a->GetPosition().DistanceSq(App->renderer3D->GetCurrentMainCamera()->GetCameraPosition()) < b->GetPosition().DistanceSq(App->renderer3D->GetCurrentMainCamera()->GetCameraPosition());
}

// ------------------------------ PARTICLE INFO ------------------------------


// -------- Global Properties --------

void ParticleSystem::SetParticleSpeed(const float& initialSpeed)
{
	particleInfo.speed = initialSpeed;
}

void ParticleSystem::SetParticlelMaxLifeTime(float maxLifeTime)
{
	particleInfo.maxLifeTime = maxLifeTime;
}

void ParticleSystem::SetParticleGlobal(bool global)
{
	particleInfo.globalTransform = global;
}


// -------- Init Properties ----------


void ParticleSystem::SetParticleInitialSize(float size)
{
	particleInfo.size = size;
}

void ParticleSystem::SetParticleInitialColor(const float4& initialColor)
{
	particleInfo.color = initialColor;
}

void ParticleSystem::SetParticleInitialForce(const float3& initialForce)
{
	particleInfo.force = initialForce;
}


// -------- Final Properties ----------


void ParticleSystem::SetParticleFinalSize(float size)
{
	endInfo.size = size;
}

void ParticleSystem::SetParticleFinalColor(const float4& initialColor)
{
	endInfo.color = initialColor;
}

void ParticleSystem::SetParticleFinalForce(const float3& initialForce)
{
	endInfo.force = initialForce;
}

// ------------------------------ PARTICLE INFO ------------------------------