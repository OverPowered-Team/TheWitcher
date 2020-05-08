#ifndef _COMPONENT_TEXT_H_
#define _COMPONENT_TEXT_H_

#include "ComponentUI.h"

#define MAX_LINES 5

class ResourceFont;
struct Character;

struct TextBackground {
	bool visible = false;
	Color color;
	uint id = 0;
};

enum TextAlign {
	NONE = -1,
	LEFT,
	MIDDLE,
	RIGHT

};

class __declspec(dllexport) ComponentText :public ComponentUI
{
public:
	ComponentText(GameObject* obj);
	~ComponentText();

	bool DrawInspector();

	void Draw(bool isGame) override;

	void Reset();
	void SetComponent(Component* component);
	void Clone(Component* clone);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void SetFont(ResourceFont* font);
	ResourceFont* GetFont() const;

	void GenerateVAOVBO();

	void SetText(const char* newText);
	const char* GetText();

	void SetAlpha(float alpha);
	void SetColor(float3 color);

private:
	TextAlign align = TextAlign::LEFT;
	ResourceFont*	font = nullptr;
	uint VAO;
	int width = 200;
	float interlineal = 1.5;
	TextBackground text_background;
	std::string	text = "Non-Text";
};

#endif // !_COMPONENT_TEXT_H_
