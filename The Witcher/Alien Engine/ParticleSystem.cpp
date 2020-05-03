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
#include "mmgr/mmgr.h"

ParticleSystem::ParticleSystem()
{
	
	particles.reserve(MAX_PARTICLES);
	emmitter.particleSystem = this;

	
	default_material = new ResourceMaterial();
	default_material->SetName("Particle Material");
	//default_material->color = particleInfo.color;
	SetMaterial(default_material);
	material->SetShader(App->resources->default_particle_shader);
	material->color = particleInfo.color;
	
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

		//// 0
		//0.0f, 0.0f,
		//// 1
		//1.0f, 0.0f,
		//// 2
		//0.0f, 1.0f,
		//// 3
		//1.0f, 1.0f,
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

	//Stop();
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
	glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_uv);


	particles.clear();

	if (material != nullptr) {

		material->DecreaseReferences();
		material = nullptr;
	}

	if (default_material) {
		default_material->DecreaseReferences();
		delete default_material;
		default_material = nullptr;
	}

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

	/*if(!std::get<0>(animation_uvs).empty())*/
	//particle->SetAnimation(std::get<0>(animation_uvs), std::get<1>(animation_uvs));

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

	ComponentCamera* mainCamera = App->renderer3D->GetCurrentMainCamera();


	//-------------------------- DRAW PARTICLES FAR TO NEAR ------------------
	for (std::vector<Particle*>::reverse_iterator iter = particles.rbegin(); iter != particles.rend(); ++iter)
	{
		(*iter)->Orientate(mainCamera);
		(*iter)->Rotate();
		(*iter)->Draw();


	}
	
	RenderLight();

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

void ParticleSystem::SetMeshType(PARTICLE_MESH type)
{
	meshType = type;
}


BillboardType ParticleSystem::GetBillboardType() const
{
	return bbType;
}

uint ParticleSystem::GetTotalParticles() const
{
	return totalParticles;
}

void ParticleSystem::CreateParticleMesh(PARTICLE_MESH type)
{

	ResourceMesh* tmp = nullptr;
	
	switch (type) {
	case PARTICLE_MESH::CUBE: {
		tmp = App->resources->GetPrimitive(PrimitiveType::CUBE);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::SPHERE: {
		tmp = App->resources->GetPrimitive(PrimitiveType::SPHERE_ALIEN);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::DODECAHEDRON: {
		tmp = App->resources->GetPrimitive(PrimitiveType::DODECAHEDRON);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::ICOSAHEDRON: {
		tmp = App->resources->GetPrimitive(PrimitiveType::ICOSAHEDRON);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::OCTAHEDRON: {
		tmp = App->resources->GetPrimitive(PrimitiveType::OCTAHEDRON);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::ROCK: {
		tmp = App->resources->GetPrimitive(PrimitiveType::ROCK);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::TORUS: {
		tmp = App->resources->GetPrimitive(PrimitiveType::TORUS);
		SetMesh(tmp);
		break; }
	case PARTICLE_MESH::CUSTOM: {
		RemoveMesh();
		break; }
	case PARTICLE_MESH::NONE: {
		RemoveMesh();
		break; }
	default:
		break;
	}


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

void ParticleSystem::SetMesh(ResourceMesh* m)
{

	if (m == nullptr)
		return;

	if (this->meshes.size() > 0)
	{
		for (int i = 0; i < this->meshes.size(); ++i)
		{
			this->meshes.at(i)->DecreaseReferences();
		}
		this->meshes.clear();

	}

	this->meshes.push_back(m);
	this->meshes.back()->IncreaseReferences();

}

void ParticleSystem::SetMeshes(std::vector<ResourceMesh*> m)
{
	if (m.size() <= 0)
		return;

	if (this->meshes.size() > 0)
	{
		for (int i = 0; i < this->meshes.size(); ++i)
		{
			this->meshes.at(i)->DecreaseReferences();
		}
		this->meshes.clear();

	}

	this->meshes = m;

	for (int i = 0; i < this->meshes.size(); ++i)
	{
		this->meshes.at(i)->IncreaseReferences();
	}
}

void ParticleSystem::RemoveMesh()
{
	if (!meshes.empty()) {

		for (int i = 0; i < this->meshes.size(); ++i)
		{
			this->meshes.at(i)->DecreaseReferences();
		}
		this->meshes.clear();

	}
	
}



//
//std::vector<uint> ParticleSystem::LoadTextureUV(int rows, int columns)
//{
//
//	
//
//	std::vector<uint> ret;
//	ret.clear();
//	int currentFrame = 0;
//	if (rows > 0 && columns > 0)
//	{
//
//		uint id_animUV = 0;
//		int currentRow = 0, currentColumn =0 ;
//		
//
//		for (int i = 0; i < rows; ++i)
//		{
//			for (int j = 0; j < columns; ++j)
//			{
//				
//
//
//				if (currentFrame >= rows * columns) {
//					currentFrame = 0;
//				}
//				else {
//					if (currentFrame >= columns)
//					{
//						currentRow = (int)(currentFrame / columns);
//					}
//					currentColumn = currentFrame % columns;
//				}
//
//				currentRow = (rows - 1) - currentRow;
//
//				float uv[] = {
//				  currentColumn / columns,       (currentRow + 1) / rows, //left top
//				  currentColumn / columns,       currentRow / rows,       //left bottom
//				  (currentColumn + 1) / columns, (currentRow + 1) / rows, //right top
//				  (currentColumn + 1) / columns, currentRow / rows,       //right bottom
//				};
//
//
//				LOG_ENGINE("Texture UV: \n%.2f %.2f\n%.2f %.2f\n%.2f %.2f\n%.2f %.2f\n", uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7])
//
//
//				glGenBuffers(1, (GLuint*)&id_animUV);
//				glBindBuffer(GL_ARRAY_BUFFER, id_animUV);
//				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);
//
//
//				ret.push_back(id_animUV);
//				currentFrame++;
//
//				glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//			}
//		}
//	}
//
//	return ret;
//
//}


	/*std::vector<uint> ret;
	ret.clear();

	if (rows > 0 && columns > 0)
	{

		 uint id_animUV = 0;

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
				
			
				glGenBuffers(1, (GLuint*)&id_animUV);
				glBindBuffer(GL_ARRAY_BUFFER, id_animUV);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);
			
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				ret.push_back(id_animUV);
			}
		}

	}
	else
	{
		ResetParticleUV();
	}*/

	

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


// ------------------------------ PARTICLE ANIMATION ------------------------------

void ParticleSystem::CalculateParticleUV(int rows, int columns, float speed, int startFrame, int endFrame)
{
	
	if (!particleInfo.frames.empty())
	{
		particleInfo.frames.clear();
		currentFrame = 0;
	}

	ResourceTexture* tex = material->GetTexture(TextureType::DIFFUSE);
	//ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(material->texturesID[(int)TextureType::DIFFUSE]);

	if (tex != nullptr)
	{
		LoadUVs(rows, columns, tex);
		SetAnimation(0, startFrame, endFrame);
		PlayAnimation(0);
		particleInfo.animSpeed = speed;
		particleInfo.animated = true;
	}
	else
	{
		LOG_ENGINE("Texture not found in particle material");
		particleInfo.animated = false;
	}


	// ------ Animation (Deprecated) --------- //

	/*if (textureUV.size() > 0)
	{
		for (int i = 0; i < textureUV.size(); ++i)
			glDeleteBuffers(1, &textureUV.at(i));

		textureUV.clear();
	}
	textureUV = LoadTextureUV(rows, columns);

	particleInfo.animation = &textureUV;
	particleInfo.animSpeed = speed;*/

	//animation_uvs = std::make_tuple(textureUV, speed);

}

void ParticleSystem::ResetParticleUV()
{

	for (uint i = 0; i < particles.size(); ++i)
	{
		particles[i]->ResetFrame();
	}


	// Update Configuration
	particleInfo.UVs[0].U = 0;
	particleInfo.UVs[0].V = 1;
	particleInfo.UVs[1].U = 1;
	particleInfo.UVs[1].V = 1;
	particleInfo.UVs[2].U = 0;
	particleInfo.UVs[2].V = 0;
	particleInfo.UVs[3].U = 1;
	particleInfo.UVs[3].V = 0;

	particleInfo.animated = false;
	particleInfo.frames.clear();
	currentFrame = 0;
	sheetWidth = 0;
	sheetHeight = 0;

}

void ParticleSystem::LoadUVs(int numRows, int numCols, ResourceTexture* tex)
{
	//int sheetWidth, sheetHeight;

	sheetWidth = tex->width;
	sheetHeight = tex->height;

	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			
			Frame newFrame;

			newFrame.x0 = (sheetWidth / numCols) * j;
			newFrame.x1 = (sheetWidth / numCols) * j + (sheetWidth / numCols);
			newFrame.y0 = (sheetHeight / numRows) * i;
			newFrame.y1 = (sheetHeight / numRows) * i + (sheetHeight / numRows);
			
			particleInfo.frames.push_back(newFrame);
			

			/*particleInfo.frames.at((i * numCols) + j).x0 = (sheetWidth / numCols) * j;
			particleInfo.frames.at((i * numCols) + j).x1 = (sheetHeight / numRows) * i;
			particleInfo.frames.at((i * numCols) + j).y0 = (sheetWidth / numCols) * j + (sheetWidth / numCols);
			particleInfo.frames.at((i * numCols) + j).y1 = (sheetHeight / numRows) * i + (sheetHeight / numRows);*/

			
		}
	}
		



}

void ParticleSystem::SetAnimation(int anim, int start, int end)
{
	particleInfo.animations[anim].startFrame = start;
	particleInfo.animations[anim].endFrame = end;

}

void ParticleSystem::PlayAnimation(int anim)
{
	particleInfo.currentAnimation = particleInfo.animations[anim];
	currentFrame = particleInfo.currentAnimation.startFrame;
}
