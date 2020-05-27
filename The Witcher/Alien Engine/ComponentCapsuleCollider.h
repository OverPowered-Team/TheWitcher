#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;

class __declspec(dllexport) ComponentCapsuleCollider : public ComponentCollider
{
public:

	friend class GameObject;

public:

	enum class Orientation { X = 0, Y = 1, Z = 2, MAX =3};

	ComponentCapsuleCollider(GameObject* go);
	void SetRotation(const float3& value);
	void SetRadius(float radius);
	float GetRadius() { return radius; }
	void SetHeight(float height);
	float GetHeight() { return height; }
	void SetOrientation(Orientation value);
	Orientation GetOrientation() { return orientation; }

private:

	void DrawSpecificInspector();
	float3 GetOrientationRotation();

	void Reset();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* clone);

private:

	float radius = 0.5f;
	float height = 1.f;

	Orientation orientation = Orientation::MAX;
};
