#include "Particle.h"
#include "ParticleSystem.h"
#include "Billboard.h"
#include "Application.h"
#include "Camera.h"
#include "GL/gl.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "ModuleObjects.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"
#include "ModuleResources.h"
Particle::Particle(ParticleSystem* owner, ParticleInfo info, ParticleMutableInfo endInfo) : owner(owner), particleInfo(info), startInfo(info), endInfo(endInfo)
{
	owner->sourceFactor = GL_SRC_ALPHA;
	owner->destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
	currentFrame = owner->currentFrame;
	
	if (owner->material != nullptr) 
	{
		p_material = new ResourceMaterial();
		p_material->SetShader(owner->material->used_shader);
		p_material->SetTexture(owner->material->GetTexture(TextureType::DIFFUSE));
		p_material->color = owner->material->color;
		p_material->shaderInputs = owner->material->shaderInputs;
		/*p_material->shaderInputs.particleShaderProperties.color = owner->material->shaderInputs.particleShaderProperties.color;
		p_material->shaderInputs.particleShaderProperties.start_color = owner->material->shaderInputs.particleShaderProperties.color;
		p_material->shaderInputs.particleShaderProperties.end_color = owner->material->shaderInputs.particleShaderProperties.end_color;*/

		/*ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(p_material->texturesID[int(TextureType::DIFFUSE)]);

		if (tex != nullptr)
		{
			sheetWidth = tex->width;
			sheetHeight = tex->height;
		}*/

	}



}

Particle::~Particle()
{
	if (owner->material != nullptr)
		delete p_material;

	currentFrame = 0;
}

void Particle::PreUpdate(float dt)
{
	currentLifeTime += dt;
	if (currentLifeTime > particleInfo.maxLifeTime)
		to_delete = true;

}

void Particle::Update(float dt)
{
	// Apply forces
	particleInfo.velocity += particleInfo.force * dt;
	
	// Move
	particleInfo.position += particleInfo.velocity * dt;


	//Rotate (Angular Velocity && Angular Acc in degrees per second)
	if (particleInfo.rotateOverTime) {

		particleInfo.angularVelocity3D += particleInfo.angularAcceleration3D * dt;

		if (particleInfo.angularVelocity3D.z >= ANGULAR_CAP)
			particleInfo.angularVelocity3D.z = ANGULAR_CAP;
		if (particleInfo.angularVelocity3D.y >= ANGULAR_CAP)
			particleInfo.angularVelocity3D.y = ANGULAR_CAP;
		if (particleInfo.angularVelocity3D.x >= ANGULAR_CAP)
			particleInfo.angularVelocity3D.x = ANGULAR_CAP;

		particleInfo.angle3D += particleInfo.angularVelocity3D * dt;
	}


	// Animation 

	if (particleInfo.animated)
	{
		animationTime += dt;

		if (animationTime > particleInfo.animSpeed)
		{
			if (currentFrame > particleInfo.currentAnimation.endFrame) {

				if (owner->emmitter.GetLoop())
				{
					currentFrame = particleInfo.currentAnimation.startFrame;
				}
				else
					currentFrame = particleInfo.currentAnimation.endFrame;

			}

			PlayFrame(currentFrame);
			currentFrame++;

			animationTime = 0.f;

		}
	}

	//Update Speed Scale
	particleInfo.velocityScale = particleInfo.speedScale * particleInfo.speed;
	
	//----------- Animation (Deprecated) ---------------- //

	/*if (particleInfo.animation != nullptr)
	{
		animationTime += dt;

		if (animationTime > particleInfo.animSpeed)
		{
			if (particleInfo.animation->size() > (currentFrame + 1))
			{
				LOG_ENGINE("Current Particle Frame: %i ", currentFrame);
				LOG_ENGINE("Current Particle UV: %i ", particleInfo.animation->at(currentFrame));

				glBindVertexArray(owner->vao);

				
				glBindBuffer(GL_ARRAY_BUFFER, particleInfo.animation->at(currentFrame));
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				
				currentFrame++;
			}
			else
				currentFrame = 0;

		
			animationTime = 0.f;
		}
	}*/

	


}

void Particle::PostUpdate(float dt)
{
	if (particleInfo.changeOverLifeTime) {

		InterpolateValues(dt);
		
	}
}

void Particle::Draw()
{
	
	// -------- ATTITUDE -------- //
	float4x4 particleLocal = float4x4::FromTRS(particleInfo.position, particleInfo.rotation, float3(particleInfo.size3D.x, particleInfo.size3D.y * (particleInfo.lengthScale + particleInfo.velocityScale), particleInfo.size3D.z));
	float4x4 particleGlobal = particleLocal;

	if (!particleInfo.globalTransform)
	{
		float4x4 parentGlobal = owner->emmitter.GetGlobalTransform();
		particleGlobal = parentGlobal * particleLocal;
	}

	glPushMatrix();
	glMultMatrixf((GLfloat*) & (particleGlobal.Transposed()));



	// ----- BLENDING COLOR ----- //
	glEnable(GL_BLEND);

	switch (owner->funcBlendSource)
	{
		case FunctionBlendType::ZERO: owner->sourceFactor = GL_ZERO; break;
		case FunctionBlendType::ONE: owner->sourceFactor = GL_ONE; break;
		case FunctionBlendType::SRC_COLOR: owner->sourceFactor = GL_SRC_COLOR; break;
		case FunctionBlendType::ONE_MINUS_SRC_COLOR: owner->sourceFactor = GL_ONE_MINUS_SRC_COLOR; break;
		case FunctionBlendType::SRC_ALPHA: owner->sourceFactor = GL_SRC_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_SRC_ALPHA: owner->sourceFactor = GL_ONE_MINUS_SRC_ALPHA; break;
		case FunctionBlendType::DST_ALPHA: owner->sourceFactor = GL_DST_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_DST_ALPHA: owner->sourceFactor = GL_ONE_MINUS_DST_ALPHA; break;
		case FunctionBlendType::DST_COLOR: owner->sourceFactor = GL_DST_COLOR; break;
		case FunctionBlendType::ONE_MINUS_DST_COLOR: owner->sourceFactor = GL_ONE_MINUS_DST_COLOR; break;
		case FunctionBlendType::CONSTANT_COLOR: owner->sourceFactor = GL_CONSTANT_COLOR; break;
		case FunctionBlendType::ONE_MINUS_CONSTANT_COLOR: owner->sourceFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
		case FunctionBlendType::CONSTANT_ALPHA: owner->sourceFactor = GL_CONSTANT_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_CONSTANT_ALPHA: owner->sourceFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
	}
	switch (owner->funcBlendDest)
	{
		case FunctionBlendType::ZERO: owner->destinationFactor = GL_ZERO; break;
		case FunctionBlendType::ONE: owner->destinationFactor = GL_ONE; break;
		case FunctionBlendType::SRC_COLOR: owner->destinationFactor = GL_SRC_COLOR; break;
		case FunctionBlendType::ONE_MINUS_SRC_COLOR: owner->destinationFactor = GL_ONE_MINUS_SRC_COLOR; break;
		case FunctionBlendType::SRC_ALPHA: owner->destinationFactor = GL_SRC_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_SRC_ALPHA: owner->destinationFactor = GL_ONE_MINUS_SRC_ALPHA; break;
		case FunctionBlendType::DST_ALPHA: owner->destinationFactor = GL_DST_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_DST_ALPHA: owner->destinationFactor = GL_ONE_MINUS_DST_ALPHA; break;
		case FunctionBlendType::DST_COLOR: owner->destinationFactor = GL_DST_COLOR; break;
		case FunctionBlendType::ONE_MINUS_DST_COLOR: owner->destinationFactor = GL_ONE_MINUS_DST_COLOR; break;
		case FunctionBlendType::CONSTANT_COLOR: owner->destinationFactor = GL_CONSTANT_COLOR; break;
		case FunctionBlendType::ONE_MINUS_CONSTANT_COLOR: owner->destinationFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
		case FunctionBlendType::CONSTANT_ALPHA: owner->destinationFactor = GL_CONSTANT_ALPHA; break;
		case FunctionBlendType::ONE_MINUS_CONSTANT_ALPHA: owner->destinationFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
	}

	
	//glBlendFunc(owner->sourceFactor, owner->destinationFactor);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch (owner->eqBlend)
	{
		case EquationBlendType::FUNC_ADD: glBlendEquation(GL_FUNC_ADD); break;
		case EquationBlendType::FUNC_SUBTRACT: glBlendEquation(GL_FUNC_SUBTRACT); break;
		case EquationBlendType::FUNC_REVERSE_SUBTRACT: glBlendEquation(GL_FUNC_REVERSE_SUBTRACT); break;
	}


	// ------ TRANSPARENCY ------ //
	/*glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, owner->alpha_test);*/

	
	// --------- COLOR --------- //
	if (p_material == nullptr)
	   glColor4f(particleInfo.color.x, particleInfo.color.y, particleInfo.color.z, particleInfo.color.w);

	
	// ------ VAO BUFFER ------ //
	
	
	//// --- VERTEX BUFFER ---- //
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, owner->id_vertex);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);





	if (owner->material != nullptr && p_material != nullptr)
	{

		owner->DeactivateLight();

		// ---- TEXTCOORD BUFFER ----- //
		/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, owner->id_uv);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);*/

		// --------- MATERIAL -------- //
		p_material->ApplyMaterial();
		


		// ---- CAMERA PROJECTION ---- //
		ComponentCamera* mainCamera = nullptr;

		if (App->objects->printing_scene)
			 mainCamera = App->camera->scene_viewport->GetCamera();
		else
			 mainCamera = App->renderer3D->GetCurrentMainCamera();

		SetUniform(owner->material, mainCamera, particleGlobal);


	}
	owner->ActivateLight();
	

	// ---- INDEX BUFFER ---- //



	// ----- DRAW ------ //

	if (owner->mesh_mode) // MESH DRAWING
	{

		if(!owner->meshes.empty())
		{
			for (int i = 0; i < owner->meshes.size(); ++i)
			{
				glBindVertexArray(owner->meshes.at(i)->vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, owner->meshes.at(i)->id_index);
				glDrawElements(GL_TRIANGLES, owner->meshes.at(i)->num_index, GL_UNSIGNED_INT, NULL);
			}
		}
	}
	else // QUAD DRAWING
	{
		glBindVertexArray(owner->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, owner->id_index);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	
	// ---- DISABLE STUFF --- //
	glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);

	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (owner->material != nullptr && p_material != nullptr)
		p_material->UnbindMaterial();

	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);
	owner->DeactivateLight();
	glPopMatrix();
	glColor4f(1.f, 1.f, 1.f, 1.f);

}

void Particle::Orientate(ComponentCamera* camera)
{
	if (camera == nullptr)
		return; 

	switch (owner->GetBillboardType())
	{
	case BillboardType::SCREEN:
		particleInfo.rotation = Billboard::AlignToScreen(camera);
		break;

	case BillboardType::WORLD:
		particleInfo.rotation = Billboard::AlignToWorld(camera, particleInfo.position);
		break;

	case BillboardType::AXIS:
		particleInfo.rotation = Billboard::AlignToAxis(camera, particleInfo.position);

		break;

	case BillboardType::VELOCITY:
		particleInfo.rotation = Billboard::AlignToVelocity(camera, particleInfo.position, particleInfo.velocity);
		break;

	case BillboardType::MESH:
		particleInfo.rotation = Quat::identity();
		break;

	case BillboardType::NONE:
		particleInfo.rotation = Quat::identity();
		break;

	default:
		break;
	}
}

void Particle::Rotate()
{
	particleInfo.rotation = particleInfo.rotation.Mul(Quat::RotateX(math::DegToRad(particleInfo.angle3D.x)));
	particleInfo.rotation = particleInfo.rotation.Mul(Quat::RotateY(math::DegToRad(particleInfo.angle3D.y)));
	particleInfo.rotation = particleInfo.rotation.Mul(Quat::RotateZ(math::DegToRad(particleInfo.angle3D.z)));
}

void Particle::InterpolateValues(float dt)
{

	rateToLerp = 1.f / particleInfo.changedTime;
	if (t <= 1)
	{
		t += rateToLerp * dt;

		if (owner->material != nullptr && p_material != nullptr)
			p_material->color = float4::Lerp(startInfo.color, endInfo.color, t);
		else
			particleInfo.color = float4::Lerp(startInfo.color, endInfo.color, t);

		//particleInfo.size = Lerp(startInfo.size, endInfo.size, t);
		particleInfo.size3D = float3::Lerp(startInfo.size3D, endInfo.size3D, t);
		particleInfo.force = float3::Lerp(startInfo.force, endInfo.force, t);
		
		if (particleInfo.speed == 0)
			return;

		particleInfo.velocity /= particleInfo.speed;
		particleInfo.speed = Lerp(startInfo.speed, endInfo.speed, t);
		particleInfo.velocity *= particleInfo.speed;
	}

}


float3 Particle::GetPosition() const
{
	return particleInfo.position;
}

ResourceMaterial* Particle::GetMaterial() const
{
	return p_material;
}

float Particle::Lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

void Particle::SetUniform(ResourceMaterial* resource_material, ComponentCamera* camera, float4x4 globalMatrix)
{
	resource_material->used_shader->SetUniformMat4f("view", camera->GetViewMatrix4x4());
	resource_material->used_shader->SetUniformMat4f("model", globalMatrix.Transposed());
	resource_material->used_shader->SetUniformMat4f("projection", camera->GetProjectionMatrix4f4());
	/*resource_material->used_shader->SetUniformFloat3("view_pos", camera->GetCameraPosition());
	resource_material->used_shader->SetUniform1i("animate", animate);*/

	resource_material->used_shader->SetUniform1i("activeFog", camera->activeFog);
	if (camera->activeFog)
	{
		resource_material->used_shader->SetUniformFloat3("backgroundColor", float3(camera->camera_color_background.r, camera->camera_color_background.g, camera->camera_color_background.b));
		resource_material->used_shader->SetUniform1f("density", camera->fogDensity);
		resource_material->used_shader->SetUniform1f("gradient", camera->fogGradient);
	}
}


// ------------------------------ PARTICLE ANIMATION ------------------------------

void Particle::UpdateUVs()
{
	glBindVertexArray(owner->vao);//Open Vertex Array

	glBindBuffer(GL_ARRAY_BUFFER, owner->id_uv);//Bind UVBO, VBO unbound
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVpoint) * 4 * 2, particleInfo.UVs, GL_STATIC_DRAW);
	
	/*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);*/

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Particle::PlayFrame(int frame)
{
	particleInfo.UVs[0].U = particleInfo.frames[frame].x0 / owner->sheetWidth;
	particleInfo.UVs[0].V = particleInfo.frames[frame].y1 / owner->sheetHeight;//up left
	particleInfo.UVs[1].U = particleInfo.frames[frame].x1 / owner->sheetWidth;
	particleInfo.UVs[1].V = particleInfo.frames[frame].y1 / owner->sheetHeight;//up right
	particleInfo.UVs[2].U = particleInfo.frames[frame].x0 / owner->sheetWidth;
	particleInfo.UVs[2].V = particleInfo.frames[frame].y0 / owner->sheetHeight;//down left
	particleInfo.UVs[3].U = particleInfo.frames[frame].x1 / owner->sheetWidth;
	particleInfo.UVs[3].V = particleInfo.frames[frame].y0 / owner->sheetHeight;//down right

	UpdateUVs();
}

void Particle::ResetFrame()
{
	//UV Data
	particleInfo.UVs[0].U = 0;
	particleInfo.UVs[0].V = 1;//
	particleInfo.UVs[1].U = 1;
	particleInfo.UVs[1].V = 1;//
	particleInfo.UVs[2].U = 0;
	particleInfo.UVs[2].V = 0;//
	particleInfo.UVs[3].U = 1;
	particleInfo.UVs[3].V = 0;//

	UpdateUVs();
	particleInfo.animated = false;
	particleInfo.frames.clear();
}

