#pragma once

#include "Component.h"
#include "Globals.h"
#include "MathGeoLib\include\MathGeoLib.h"

class GameObject;

class ComponentReverbZone : public Component
{
public:
	ComponentReverbZone(GameObject* parent);
	~ComponentReverbZone();

	void Update();
	void SetRadius(float rad);
	void DebugDrawSphere();

	void SaveReverbZone(JSON_Array* componentsObj) const;
	void LoadReverbZone(JSON_Object* obj, GameObject* go);

public:
	float radius = 10.0f;

private:
	math::Sphere sphere;

};
