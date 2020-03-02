#pragma once
#include "Globals.h"

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/quat.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include "ResourceTexture.h"

class ParticleSystem;
class ComponentCamera;

#define DEGTORAD 0.0174532925199432957f // 1degree x (pi rads / 180 degrees) = 0.017 rads
#define RADTODEG 57.295779513082320876f // 1rad x (180 degrees / pi rads) = 57 degrees
#define ANGULAR_CAP 200 // angular velocity will be capped at 360 degrees x second

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

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 1.f;
	float4 lightColor = float4::zero;

	float maxLifeTime = 5.f;

	bool globalTransform = true;
	bool changeOverLifeTime = false;

	bool rotateOverTime = false;
	bool axisRot3D = false;
	bool axisRot3DStart = false;
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
		
	}

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 1.f;
	float4 lightColor;
	float3 force = float3(0.f, -10.f, 0.f);  // float3::zero;	
	

	
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

	float Lerp(float v0, float v1, float t);

public:

	bool to_delete = false;

private:
	//ResourceTexture* resMat = nullptr;

	ParticleSystem* owner = nullptr;

	ParticleInfo particleInfo;
	ParticleMutableInfo startInfo;
	ParticleMutableInfo endInfo;

	float currentLifeTime = 0.f;


	// -------- Lerping -------------

	float rateToLerp = 0.f;
	float t = 0.0f;

};