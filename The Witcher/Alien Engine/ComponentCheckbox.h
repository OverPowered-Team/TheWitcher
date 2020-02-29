#ifndef _COMPONENT_CHECKBOX_H_
#define _COMPONENT_CHECKBOX_H_

#include "ComponentUI.h"
#include "Color.h"
#include <functional>

class ResourceTexture;
class ComponentCanvas;

class __declspec(dllexport) ComponentCheckbox :public ComponentUI
{
	friend class ReturnZ;
	friend class CompZ;
public:

	ComponentCheckbox(GameObject* obj);
	~ComponentCheckbox() {};

	bool DrawInspector();

	void Draw(bool isGame) override;
	void DrawTexture(bool isGame, ResourceTexture* tex);

	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };

	void SetActive(bool active);

	void AddListenerOnHover(std::function<void()> funct);
	void AddListenerOnClick(std::function<void()> funct);
	void AddListenerOnClickRepeat(std::function<void()> funct);
	void AddListenerOnRelease(std::function<void()> funct);

	//bool CheckMouseInsideCheckbox(float3 mouse_pos);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	void CallListeners(std::vector<std::function<void()>>* listeners);

private:
	bool active = true;
	bool clicked = false;
	float crossScaleX = 0.5F;
	float crossScaleY = 0.5F;
	float tickScaleX = 0.5F;
	float tickScaleY = 0.5F;

private:
	std::vector<std::function<void()>> listenersOnHover;
	std::vector<std::function<void()>> listenersOnClick;
	std::vector<std::function<void()>> listenersOnClickRepeat;
	std::vector<std::function<void()>> listenersOnRelease;

	ComponentCanvas* GetCanvas();

	ResourceTexture* crossTexture = nullptr;
	ResourceTexture* tickTexture = nullptr;
};









#endif // !_COMPONENT_CHECKBOX_H_
