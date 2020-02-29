#pragma once

#include "ModuleImporter.h"
#include "Component.h"
#include "Color.h"
#include "TextEdit/TextEditor.h"

class ResourceTexture;
class ResourceShader;

class __declspec(dllexport) ComponentMaterial : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleImporter;
	friend class ResourceMesh;
public:
	ComponentMaterial(GameObject* attach);
	virtual ~ComponentMaterial();

private:
	void BindTexture();
	bool DrawInspector();

	void InspectorShaderProperties();

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* clone);

	const ResourceTexture* GetTexture() const;

public:
	void SetTexture(ResourceTexture* tex);


	Color color{ 1,1,1,1 };
	bool texture_activated = true;
public:
	ResourceTexture* texture = nullptr;

private:
	bool change_texture_menu = false;

// Shaders
public:
	void ShowShaderTextEditor();
	ResourceShader* used_shader = nullptr;

private:
	TextEditor shader_editor;
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
	std::string file_to_edit;
	bool show_shader_text_editor = false;
	bool select_shader = false;
};