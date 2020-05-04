#ifndef _COMPONENT_BUTTON_H_
#define _COMPONENT_BUTTON_H_

#include "ComponentUI.h"
#include "Color.h"
#include "Event.h"
#include <functional>

class ResourceTexture;

struct AnimationInfo
{
	std::vector<ResourceTexture*> tex_array;
	float current_frame = 0.0f;
	int last_frame = 0;
	int loops = 0;
	bool loop = true;
	float speed = 1.0f;
};

class __declspec(dllexport) ComponentButton :public ComponentUI
{
	friend class ModuleObjects;
public:
	ComponentButton(GameObject* obj);
	~ComponentButton() {};

	void SetActive(bool active);
	void Draw(bool isGame);
	void AddListenerOnHover(std::string name, std::function<void()> funct);
	void AddListenerOnClick(std::string name, std::function<void()> funct);
	void AddListenerOnClickRepeat(std::string name, std::function<void()> funct);
	void AddListenerOnRelease(std::string name, std::function<void()> funct);
	void AddListenerOnExit(std::string name, std::function<void()> funct);
	void AddListenerOnEnter(std::string name, std::function<void()> funct);

	bool CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name);

	//void RemoveListenerOnHover(std::function<void()> funct);
	//void RemoveListenerOnClick(std::function<void()> funct);
	//void RemoveListenerOnClickRepeat(std::function<void()> funct);
	//void RemoveListenerOnRelease(std::function<void()> funct);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void SetAnimSpeed(float speed, UIState type);
	float GetAnimSpeed(UIState type);

protected:
	void HandleAlienEvent(const AlienEvent& e);


private:
	bool DrawInspector();

	bool OnEnter();
	bool OnIdle();
	bool OnHover();
	bool OnClick();
	bool OnPressed();
	bool OnRelease();
	bool OnExit();


	void CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners);
	/*void SetStateTexture(UIState state, ResourceTexture* tex);
	void ClearStateTexture(UIState state);*/

	ResourceTexture* ClearTextureArray(ResourceTexture* item);
	ResourceTexture* SetTextureArray(ResourceTexture* tex, ResourceTexture* item);
	void SetCurrentTexArray(AnimationInfo* new_tex);

	ResourceTexture* GetCurrentFrame(float dt);
	bool Finished() const;
	void Reset();
	int SeeCurrentFrame();

public:

	Color idle_color = { 0.8f,0.8f,0.8f,1.0f };
	Color hover_color = { 1.0f,1.0f,1.0f,1.0f };
	Color clicked_color = { 0.7f,0.7f,0.7f,1.0f };
	Color pressed_color = { 0.75f,0.75f,0.75f,1.0f };
	Color disabled_color = { 0.3f,0.3f,0.3f,1.0f };

protected:

	AnimationInfo idle_info;
	AnimationInfo hover_info;
	AnimationInfo clicked_info;
	AnimationInfo pressed_info;
	AnimationInfo disabled_info;
	AnimationInfo* current_tex_array = nullptr;


private:

	std::vector<std::pair<std::string, std::function<void()>>> listenersOnHover;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClick;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnClickRepeat;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnRelease;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnExit;
	std::vector<std::pair<std::string, std::function<void()>>> listenersOnEnter;

	bool active_ui = true;
};


#endif // !_COMPONENT_BUTTON_H