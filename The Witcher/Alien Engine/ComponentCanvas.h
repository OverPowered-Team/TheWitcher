#ifndef _COMPONENT_CANVAS_H_
#define _COMPONENT_CANVAS_H_

#include "Component.h"

class GameObject;
class ResourceShader;

class __declspec(dllexport) ComponentCanvas :public Component {
public:
	ComponentCanvas(GameObject* obj);
	~ComponentCanvas();

	void PreUpdate() {};
	void Update() {};
	void PostUpdate() {};

	bool DrawInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Draw();

public:
	uint width = 0, height = 0;
	bool allow_navigation = true;
};


#endif // !_COMPONENT_CANVAS_H_