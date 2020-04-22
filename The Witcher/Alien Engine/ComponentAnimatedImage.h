#ifndef _COMPONENT_ANIMATED_IMAGE_H_
#define _COMPONENT_ANIMATED_IMAGE_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentAnimatedImage :public ComponentUI
{
public:
	ComponentAnimatedImage(GameObject* obj);
	~ComponentAnimatedImage() {};

	bool DrawInspector();

	void Draw(bool isGame);

	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	ResourceTexture* ClearTextureArray(ResourceTexture* item);
	ResourceTexture* SetTextureArray(ResourceTexture* tex, ResourceTexture* item);

	void SetAnimSpeed(float speed);
	float GetAnimSpeed();
private:
	ResourceTexture* GetCurrentFrame(float dt);
	bool Finished() const;
	void Reset();
	int SeeCurrentFrame();

private:
	std::vector<ResourceTexture*> images;

	bool loop = true;
	float speed = 1.0f;

	float current_frame = 0.0f;
	int last_frame = 0;
	int loops = 0;
};

#endif // !_COMPONENT_ANIMATED_IMAGE_H_

