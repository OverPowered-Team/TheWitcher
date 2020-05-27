#pragma once

#include "ModuleImporter.h"
#include "Component.h"
#include "Color.h"
#include "TextEdit/TextEditor.h"

class ResourceTexture;
class ResourceShader;
enum class TextureType;

class __declspec(dllexport) ComponentMaterial : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class GameObject;
	friend class ModuleImporter;
	friend class ResourceMesh;
	friend class ModuleTween;
	friend class AnimTween;
public:
	ComponentMaterial(GameObject* attach);
	virtual ~ComponentMaterial();

private:
	bool DrawInspector();

	void Reset();
	void SetComponent(Component* component);

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* clone);


public:
	void SetTexture(ResourceTexture* tex, TextureType texType);
	void RemoveTexture(TextureType texType);
	const ResourceTexture* GetTexture(TextureType texType) const;

	void SetMaterial(ResourceMaterial* mat);
	const ResourceMaterial* GetMaterial() const;

	ResourceShader* GetUsedShader() const; 

	bool IsTransparent() const;

private:
	bool change_texture_menu = false;

// Shaders
public:
	ResourceMaterial* material = nullptr; 

private:
	//TextEditor shader_editor;
	//TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
	std::string file_to_edit;
	bool show_shader_text_editor = false;
	bool select_shader = false;

};