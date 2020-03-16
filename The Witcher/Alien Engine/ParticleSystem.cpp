#include "ParticleSystem.h"
#include "Application.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "Camera.h"
#include "ResourceShader.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "GL/gl.h"

ParticleSystem::ParticleSystem()
{
	particles.reserve(MAX_PARTICLES);
	emmitter.particleSystem = this;

	
	material = App->resources->default_material;
	material->IncreaseReferences();
	material->SetShader(App->resources->default_particle_shader);

	
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
		0.0f, 0.0f, 
		// 1
		1.0f, 0.0f,
		// 2
		0.0f, 1.0f,
		// 3
		1.0f, 1.0f,
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

	// LIGHTS
	light_id = GL_LIGHT0;
	InitLight();
}


ParticleSystem::~ParticleSystem()
{
	// Delete all particles
	for (std::vector<Particle*>::iterator iter = particles.begin(); iter != particles.end(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}

	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_uv);

	particles.clear();

	if (material != nullptr)
		material = nullptr;

	DeactivateLight();
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


		if (!emmitter.isDelayed()) {

			int particlesToCreate = emmitter.GetParticlesToInstantiate();
			InstantiateParticles(particlesToCreate);

			if (emmitter.HasBurstsActive())
			{
				int particlesToBurst = emmitter.GetParticlesToBurst();
				InstantiateParticles(particlesToBurst);
			}
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

	Particle* particle = new Particle(this, info, endInfo);

	if(std::get<0>(animation_uvs).size() > 0)
		particle->SetAnimation(std::get<0>(animation_uvs), std::get<1>(animation_uvs));

	particles.push_back(particle);
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

	RenderLight();
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
	emmitter.playing = true;
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

void ParticleSystem::Stop()
{
	ResetSystem();
	Pause();
}

void ParticleSystem::ResetSystem()
{
	// Reset Emmitter
	emmitter.Reset();
	emmitter.current_delay = 0.f;

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

void ParticleSystem::StopEmmitter()
{
	emmitter.Stop();
	
}

void ParticleSystem::StartEmmitter()
{
	emmitter.Play();
	
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

void ParticleSystem::SetMaterial(ResourceMaterial* mat)
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

void ParticleSystem::RemoveMaterial()
{
	material->DecreaseReferences();
	material = nullptr;
}

void ParticleSystem::CalculateParticleUV(int rows, int columns, float speed)
{
	std::vector<uint> textureUV = LoadTextureUV(rows, columns);
	animation_uvs = std::make_tuple(textureUV, speed);
}

std::vector<uint> ParticleSystem::LoadTextureUV(int rows, int columns)
{

	std::vector<uint> ret;

	if (rows > 0 && rows > 0)
	{

		uint texID = 0;

		float r_scale = 1.0f / rows;
		float c_scale = 1.0f / columns;


		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < columns; ++j)
			{
				float uv[]
				{
					j * c_scale,					1.0f - (i * r_scale + r_scale),
					j * c_scale + c_scale,			1.0f - (i * r_scale + r_scale),
					j * c_scale,					1.0f -  i * r_scale,
					j * c_scale + c_scale,			1.0f -  i * r_scale,
				};

			
				LOG_ENGINE("Texture UV: \n%.2f %.2f\n%.2f %.2f\n%.2f %.2f\n%.2f %.2f\n", uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7])
				
			
				glGenBuffers(1, (GLuint*)&texID);
				glBindBuffer(GL_ARRAY_BUFFER, texID);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);
			
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				ret.push_back(texID);
			}
		}

	}

	return ret;

}

// ------------------------------ PARTICLE LIGHT ------------------------------

void ParticleSystem::InitLight()
{
	glLightfv(light_id, GL_AMBIENT, &ambient);
	glLightfv(light_id, GL_DIFFUSE, &diffuse);
}

void ParticleSystem::RenderLight()
{
	float pos[] = { emmitter.GetPosition().x, emmitter.GetPosition().x, emmitter.GetPosition().x, 1.F };
	glLightfv(light_id, GL_POSITION, pos);
}

void ParticleSystem::ActivateLight()
{
	glEnable(light_id);
}

void ParticleSystem::DeactivateLight()
{
	glDisable(light_id);
}