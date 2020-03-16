#ifndef _COMPONENT_BUTTON_H_
#define _COMPONENT_BUTTON_H_

#include "ComponentUI.h"
#include "Color.h"
#include <functional>

class ResourceTexture;

class __declspec(dllexport) ComponentButton :public ComponentUI
{
	friend class ModuleObjects;
public:
	ComponentButton(GameObject* obj);
	~ComponentButton() {};

	void SetActive(bool active);

	void AddListenerOnHover(std::string name, std::function<void()> funct);
	void AddListenerOnClick(std::string name, std::function<void()> funct);
	void AddListenerOnClickRepeat(std::string name, std::function<void()> funct);
	void AddListenerOnRelease(std::string name, std::function<void()> funct);
	bool CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name);

	//void RemoveListenerOnHover(std::function<void()> funct);
	//void RemoveListenerOnClick(std::function<void()> funct);
	//void RemoveListenerOnClickRepeat(std::function<void()> funct);
	//void RemoveListenerOnRelease(std::function<void()> funct);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

private:
	bool DrawInspector();

	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();

	void CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners);

public:

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };

private:

	std::vector<std::pair<std::string, std::function<void()>>> listenersOnHover;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClick;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClickRepeat;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnRelease;

	bool active = true;
};


#endif // !_COMPONENT_BUTTON_H
