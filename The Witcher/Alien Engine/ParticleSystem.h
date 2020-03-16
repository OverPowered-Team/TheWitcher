#pragma once

#include "Globals.h"
#include <vector>
#include "Billboard.h"
#include "ResourceTexture.h"
#include "ParticleEmitter.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"
#include "ComponentParticleSystem.h"
#include <map>

#include "Particle.h"

#define MAX_PARTICLES 10000
#define MAX_PARTICLES_TO_BURST 500

enum class EquationBlendType
{
	FUNC_ADD,
	FUNC_SUBTRACT,
	FUNC_REVERSE_SUBTRACT
};

enum class FunctionBlendType
{
	ZERO,
	ONE,
	SRC_COLOR,
	ONE_MINUS_SRC_COLOR,
	DST_COLOR,
	ONE_MINUS_DST_COLOR,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	DST_ALPHA,
	ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA

};

bool compareParticles(Particle* a, Particle* b);

class __declspec(dllexport) ParticleSystem
{
	friend class ComponentParticleSystem;

public:

	ParticleSystem();
	~ParticleSystem();

private: 

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	void CreateParticle(ParticleInfo info, ParticleMutableInfo endInfo);
	void InstantiateParticles(int particles);
	void DrawParticles();
	void DrawPointsForParticles();

	void DrawEmmitter();

public: 


	bool isSystemActive() const;

	bool isPlaying() const;
	void Play();
	void Pause();
	void Restart();
	void Stop();
	void ResetSystem();
	void InitLight();
	void RenderLight();
	void ActivateLight();
	void DeactivateLight();
	void StopEmmitter();
	void StartEmmitter();

	void SetBillboardType(BillboardType type);
	BillboardType GetBillboardType() const;
	uint GetTotalParticles() const;

	// ------------------------------ PARTICLE INFO ------------------------------

	// -------- Global Properties --------

	void SetParticleSpeed(const float& initialSpeed);
	void SetParticlelMaxLifeTime(float maxLifeTime);
	void SetParticleGlobal(bool global);

	// -------- Init Properties ----------

	void SetParticleInitialSize(float size);
	void SetParticleInitialColor(const float4& initialColor);
	void SetParticleInitialForce(const float3& initialForce);

	// -------- Final Properties ----------

	void SetParticleFinalSize(float size);
	void SetParticleFinalColor(const float4& initialColor);
	void SetParticleFinalForce(const float3& initialForce);

	// ---------------------------------------------------------------------------
	void SetMaterial(ResourceMaterial* mat);
	void RemoveMaterial();
	
	void CalculateParticleUV(int rows, int columns);
	std::vector<uint> LoadTextureUV(int rows, int columns);
public: 

	ParticleEmmitter emmitter;
	BillboardType bbType = BillboardType::SCREEN;

private:

	ParticleInfo particleInfo;
	ParticleMutableInfo endInfo;

	bool playing = true;
	//std::map<float, Particle*> sortedParticles;
	std::vector<Particle*> particles;
	uint totalParticles = 0u;
	

public:
	ResourceTexture* texture = nullptr;
	ResourceMaterial* material = nullptr;
	EquationBlendType eqBlend = EquationBlendType::FUNC_ADD;
	FunctionBlendType funcBlendSource = FunctionBlendType::SRC_ALPHA;
	FunctionBlendType funcBlendDest = FunctionBlendType::ONE_MINUS_SRC_ALPHA;

	int sourceFactor;
	int destinationFactor;
	float alpha_test = 0.1f;

	// Vertex Array Object
	uint vao = 0u;

	// Buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_uv = 0;

	// Light
	Color ambient{ 1.f, 1.f, 1.f, 1.0f };
	Color diffuse{ 1.f, 1.f, 1.f, 1.0f };
	uint light_id = 0;

	// UV Buffer ids
	std::vector<uint> id_uvs;
};