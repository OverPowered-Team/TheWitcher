#ifndef _COMPONENT_CANVAS_H_
#define _COMPONENT_CANVAS_H_

#include "Component.h"
#include "Billboard.h"

class GameObject;
class ResourceShader;


class __declspec(dllexport) ComponentCanvas :public Component {
public:
	ComponentCanvas(GameObject* obj);
	~ComponentCanvas();

	void PreUpdate() {};
	void Update() {};
	void PostUpdate() {};

	void DrawScene(ComponentCamera* camera) override;
	bool DrawInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Draw();

	float3 GetWorldPositionInCanvas(const float3& world_position);

public:
	uint width = 0, height = 0;
	bool allow_navigation	= true;
	bool isWorld			= false;
	BillboardType bbtype	= BillboardType::SCREEN;
};


#endif // !_COMPONENT_CANVAS_H_