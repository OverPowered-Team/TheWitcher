#pragma once
#include "Globals.h"

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/quat.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include "ResourceTexture.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

class ParticleSystem;
class ComponentCamera;


#define ANGULAR_CAP 200 // angular velocity will be capped at 360 degrees x second
#define MAX_ANIMATIONS 16

enum class PARTICLE_MESH
{
	UNKNOWN = -1,
	CUBE,
	SPHERE,
	ROCK,
	DODECAHEDRON,
	OCTAHEDRON,
	TORUS,
	ICOSAHEDRON,

	CUSTOM,
	NONE
};


struct Frame {
	float x0, x1, y0, y1;
};

struct UVpoint {
	float U, V;
};

struct Animation {
	int startFrame, endFrame = 0;
};

struct ParticleInfo
{
	float3 position = float3::zero();
	Quat rotation = Quat::identity();
	float3 velocity = float3(0.f, 5.f, 0.f); // float3::zero;

	float3 angularVelocity3D = float3::zero();
	float3 angle3D = float3::zero();
	float3 angularAcceleration3D = float3::zero();

	float3 force = float3(0.f, 0.0f, 0.f); // float3::zero;
	float speed = 1.0f;

	float4 color = float4(1.0f, 0.0f, 0.8f, 1.0f); // default pink
	float size = 1.f;
	float3 size3D = float3(1.f, 1.f, 1.f);
	float4 lightColor = float4::zero;

	float maxLifeTime = 5.f;
	float changedTime = 5.f;

	bool globalTransform = true;
	bool changeOverLifeTime = false;

	bool rotateOverTime = false;
	bool axisRot3D = false;
	bool axisRot3DStart = false;
	bool size3DStart = false;
	
	// Animation
	UVpoint UVs[4];
	std::vector<Frame> frames;
	Animation currentAnimation;
	Animation animations[MAX_ANIMATIONS];
	float animSpeed = 0.f;
	bool animated = false;
	//int currentFrame = 0;

	//Stretch billboard
	float lengthScale = 1.0f;
	float speedScale = 0.0f;
	float velocityScale = 0.0f;
};

struct ParticleMutableInfo
{
	ParticleMutableInfo() {}
	ParticleMutableInfo(const ParticleInfo& p)
	{
		color = p.color;
		size = p.size;
		lightColor = p.lightColor;
		force = p.force;
		speed = p.speed;
		size3D = p.size3D;
	}

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 1.f;
	float3 size3D = float3(1.f, 1.f, 1.f);
	float4 lightColor;
	float3 force = float3(0.f, -10.f, 0.f);  // float3::zero;	
	float speed = 1.0f;
	
	
};

class Particle
{

public:

	Particle(ParticleSystem* owner, ParticleInfo info, ParticleMutableInfo endInfo);
	~Particle();

	void PreUpdate(float dt);
	void Update(float dt);
	void PostUpdate(float dt);

	void Draw();
	void Orientate(ComponentCamera* camera);
	void Rotate();

	void InterpolateValues(float dt);

	float3 GetPosition() const;
	ResourceMaterial* GetMaterial() const;

	float Lerp(float v0, float v1, float t);
	void SetUniform(ResourceMaterial* resource_material, ComponentCamera* camera, float4x4 globalMatrix);

	//Animation
	void UpdateUVs();
	void PlayFrame(int frame);
	void ResetFrame();

public:

	bool to_delete = false;

private:
	//ResourceTexture* resMat = nullptr;
	
	ParticleSystem* owner = nullptr;
	ResourceMaterial* p_material = nullptr;

	ParticleInfo particleInfo;
	ParticleMutableInfo startInfo;
	ParticleMutableInfo endInfo;

	float currentLifeTime = 0.f;
	float animationTime = 0.f;
	int currentFrame = 0;

	// -------- Lerping -------------

	float rateToLerp = 0.f;
	float t = 0.0f;
	//float sheetWidth, sheetHeight = 0.f;

};