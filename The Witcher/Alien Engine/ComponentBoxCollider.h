#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;


class __declspec(dllexport) ComponentBoxCollider : public ComponentCollider
{
	friend class GameObject;

public:

	ComponentBoxCollider(GameObject* go);

	void SetSize(const float3 size);
	const float3 GetSize() const { return size; }

private :

	void ScaleChanged();
	void DrawSpecificInspector();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void Clone(Component* clone);
	void Reset();

	const float3 CalculateSize();
	PxShape* ReCreateBoxShape();

private:

	float3 size = float3::one();
};
