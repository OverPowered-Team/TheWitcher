#ifndef _COMPONENT_CHECKBOX_H_
#define _COMPONENT_CHECKBOX_H_

#include "ComponentUI.h"
#include "Color.h"
#include "Event.h"
#include <functional>

class ResourceTexture;
class ComponentCanvas;

class __declspec(dllexport) ComponentCheckbox :public ComponentUI
{
	friend class ReturnZ;
	friend class CompZ;
	friend class ModuleObjects;
public:

	ComponentCheckbox(GameObject* obj);
	~ComponentCheckbox() {};

	bool DrawInspector();

	void Draw(bool isGame) override;
	void DrawTexture(bool isGame, ResourceTexture* tex, bool background = true);

	bool OnIdle();
	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();
	bool OnExit();
	bool OnEnter();

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };

	Color checkbox_current_color = { 0.8f,0.8f,0.8f,1.0f };
	Color checkbox_idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color checkbox_hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color checkbox_clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color checkbox_pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color checkbox_disabled_color = { 0.3f,0.3f,0.3f,1.0f };

	void SetActive(bool active);

	void AddListenerOnHover(std::string name, std::function<void()> funct);
	void AddListenerOnClick(std::string name, std::function<void()> funct);
	void AddListenerOnClickRepeat(std::string name, std::function<void()> funct);
	void AddListenerOnRelease(std::string name, std::function<void()> funct);
	void AddListenerOnExit(std::string name, std::function<void()> funct);
	void AddListenerOnEnter(std::string name, std::function<void()> funct);

	bool CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name);

	//bool CheckMouseInsideCheckbox(float3 mouse_pos);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);
	bool GetStatus();

protected:
	void HandleAlienEvent(const AlienEvent& e);
private:
	void CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners);

private:
	bool clicked = false;
	float crossScaleX = 0.5F;
	float crossScaleY = 0.5F;
	float tickScaleX = 0.5F;
	float tickScaleY = 0.5F;

private:
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnHover;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClick;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClickRepeat;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnRelease;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnExit;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnEnter;

	ComponentCanvas* GetCanvas();

	ResourceTexture* crossTexture = nullptr;
	ResourceTexture* tickTexture = nullptr;

	bool active_ui = true;
};









#endif // !_COMPONENT_CHECKBOX_H_
