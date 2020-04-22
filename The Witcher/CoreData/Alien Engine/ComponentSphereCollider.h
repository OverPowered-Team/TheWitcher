#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"

class GameObject;
class ComponentMesh;

class ComponentSphereCollider : public ComponentCollider
{
public:

	friend class GameObject;

public:

	ComponentSphereCollider(GameObject* go);

	/*virtual void QueryMesh(ComponentMesh* mesh) override;*/
	void SetRadius(float radius);

protected:

	void ScaleChanged();

private:

	void InitializeRadius();
	const float CalculateRadius();
	PxShape* RecreateSphereShape();

	void DrawSpecificInspector();
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	void Reset();

	void Clone(Component* clone);

private:

	float radius = 0.5f;
};