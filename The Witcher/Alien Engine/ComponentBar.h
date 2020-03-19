#ifndef _COMPONENT_BAR_H_
#define _COMPONENT_BAR_H_

#include "ComponentUI.h"

class ResourceTexture;

class __declspec(dllexport) ComponentBar :public ComponentUI
{
	friend class ReturnZ;
	friend class CompZ;
public:
	ComponentBar(GameObject* obj);
	~ComponentBar() {};

	bool DrawInspector();

	void Draw(bool isGame) override;
	void DrawTexture(bool isGame, ResourceTexture* tex);

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	//Script
	void CalculateFactor();
	float GetBarValue();
	void SetBarValue(float factor);
	void SetBarColor(float r, float g, float b, float a);
	void SetTexture(ResourceTexture* tex);

public:
	float currentValue = 0.0f, maxValue = 100.0f, minValue = 0.0f;
	Color bar_color = { 1.0f, 1.0f,1.0f,1.0f };

private:
	ResourceTexture* barTexture = nullptr;
	float factor = 0.0f;
	float barScaleY = 0.8F, barScaleX = 1.0f;

	float offsetX = 0.0f, offsetY = 0.0f;
};

#endif // !_COMPONENT_BAR_H_
