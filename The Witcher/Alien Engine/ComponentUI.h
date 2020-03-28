#ifndef _COMPONENT_UI_H_
#define _COMPONENT_UI_H_

#include "Component.h"
#include "ComponentCanvas.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"
#include "Color.h"

class ResourceTexture;
class ReosurceFont;

enum UIState {
	Idle,
	Hover,
	Click,
	Pressed,
	Release,
	Disabled
};

class __declspec(dllexport) ComponentUI :public Component {
	friend class GameObject;
	friend class ComponentCanvas;
	friend class ComponentImage;
	friend class ComponentCheckbox;
	friend class ComponentText;
	friend class ComponentButton;
	friend class ComponentSlider;
	friend class ComponentBar;
	friend class ComponentImage;
	friend class ModuleObjects;
	friend class Component;
	friend class ReturnZ;
	friend class CompZ;
public:
	ComponentUI(GameObject* obj);

	virtual ~ComponentUI();

	virtual void ChangeVertex(float width, float height);
	void UpdateVertex();

	virtual bool OnIdle() { return true; };
	virtual bool OnHover() { return true; };
	virtual bool OnClick() { return true; };
	virtual bool OnPressed() { return true; };
	virtual bool OnRelease() { return true; };

	void SetCanvas(ComponentCanvas* canvas_);

	virtual void Update();
	virtual void Draw(bool isGame);

	virtual void SaveComponent(JSONArraypack* to_save) {};
	virtual void LoadComponent(JSONArraypack* to_load) {};

	void ClearTexture();
	void SetTexture(ResourceTexture* tex);

	virtual bool CheckMouseInside(float3 mouse_pos);
	virtual void UILogicMouse();
	virtual void UILogicGamePad();
	virtual void SetSize(float width, float height);

	//Script
	void SetBackgroundColor(float r, float g, float b, float a);


private:
	void CheckFirstSelected();
	
	
public:
	Color current_color = { 0.8f,0.8f,0.8f,1.0f };

protected:
	float x = 0.0f, y = 0.0f;

	ComponentType ui_type = ComponentType::UI;
	UIState state = Idle;

	ResourceTexture* texture = nullptr;

	float3 vertices[4]{ {-1,1,0},{-1,-1,0},{1,-1,0},{1,1,0} }; 
	float2 uv[4]{ {-1,-1},{-1,0},{0,0},{0,-1} };
	uint index[6]{ 0,1,2,2,3,0 };
	uint indexID = 0;
	uint verticesID = 0;
	uint uvID = 0;

	ComponentCanvas* canvas				= nullptr;
	ComponentTransform* canvas_trans	= nullptr;

	float2 size = { 10,10 };

	//UI FOCUS
	
	bool tabbable			= false;
	u64 select_on_up		= -1;
	u64 select_on_down		= -1;
	u64 select_on_left		= -1;
	u64 select_on_right		= -1;



};


#endif