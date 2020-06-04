#ifndef _COMPONENT_BAR_H_
#define _COMPONENT_BAR_H_

#include "ComponentUI.h"

class ResourceTexture;

enum class SCISSOR_TYPE {
	RIGHT_TO_LEFT,
	LEFT_TO_RIGHT,
	CENTER
};

class __declspec(dllexport) ComponentBar :public ComponentUI
{
	friend class ReturnZ;
	friend class CompZ;
public:
	ComponentBar(GameObject* obj);
	~ComponentBar();

	bool DrawInspector();

	void Draw(bool isGame) override;
	void DrawTexture(bool isGame, ResourceTexture* tex);

	void Reset() {};
	void SetComponent(Component* component) {};
	void Clone(Component* clone) {};

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void SetSize(float width, float height);

	//Script
	void CalculateFactor();
	float GetBarValue();
	void SetBarValue(float factor);
	void SetBarColor(float r, float g, float b, float a);
	void SetTextureBar(ResourceTexture* tex);

public:
	float currentValue = 0.0f, maxValue = 100.0f, minValue = 0.0f;
	Color bar_color = { 1.0f, 1.0f,1.0f,1.0f };

private:
	ResourceTexture* barTexture = nullptr;
	float factor = 0.0f;
	float barScaleY = 0.8F, barScaleX = 1.0f;

	float offsetX = 0.0f, offsetY = 0.0f;
	bool draw_bar = false;
	bool right_to_left = true;
	SCISSOR_TYPE scType = SCISSOR_TYPE::RIGHT_TO_LEFT;
};

#endif // !_COMPONENT_BAR_H_
