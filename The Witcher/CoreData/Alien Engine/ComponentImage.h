#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentImage :public ComponentUI
{
	friend class ComponentBar;
public:
	ComponentImage(GameObject* obj);
	~ComponentImage() {};

	bool DrawInspector();

	void Reset();
	void SetComponent(Component* component);
	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

};

#endif // !_COMPONENT_IMAGE_H_
