#pragma once

#include "Component.h"
#include <vector>
#include <functional>
#include <string>
#include <map>

class Prefab;

struct __declspec(dllexport) InspectorScriptData {
	friend class ComponentScript;
	enum DataType {
		INT, // DONE
		INT2,
		INT3,
		FLOAT, // DONE
		FLOAT2,
		FLOAT3,
		STRING,
		BOOL, // DONE
		PREFAB, // DONE
		GAMEOBJECT, // DONE
		TOOL_TIP,
		TEXT,
		SEPARATOR,
		SPACING,
		ENUM,
	};

	enum ShowMode {
		NONE,
		INPUT_INT,
		DRAGABLE_INT,
		SLIDER_INT,
		INPUT_FLOAT,
		DRAGABLE_FLOAT,
		SLIDER_FLOAT,
		CHECKBOX,
	};

	InspectorScriptData(const std::string& variable_name, const DataType& variable_type, void* ptr, const ShowMode& mode) {
		this->variable_name = variable_name;
		this->variable_type = variable_type;
		this->ptr = ptr;
		this->show_as = mode;
	}

	std::string variable_name;
	DataType variable_type;
	ShowMode show_as;
	void* ptr = nullptr;

	const char* previewEnumName = nullptr;
	std::vector<std::pair<u64, std::string>> enumNames;

	GameObject** obj = nullptr;
private:
	//ugly
	float min_slider = 0;
	float max_slider = 0;
};

class __declspec(dllexport) ComponentScript : public Component {
	friend class ReturnZ;
	friend class CompZ;
	friend class PanelInspector;
	friend class Prefab;
	friend class PanelHierarchy;
	friend class PanelAnimTimeline;
	friend class ResourceAnimatorController;
	friend class ComponentButton;
	friend class ComponentCheckbox;
	friend class ModuleObjects;
	friend class ComponentTransform;
	friend class ComponentPhysics;
	friend class SimulationEventCallback;
	friend class ComponentCollider;
	friend class GameObject;
	friend class ComponentCharacterController;
public:
	ComponentScript(GameObject* attach);
	virtual ~ComponentScript();

private:

	void Reset();
	void SetComponent(Component* component);

	bool DrawInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

	void Clone(Component* clone);

	void OnDisable();
	void OnEnable();

	void LoadData(const char* name, bool is_alien);

	static std::string GetVariableName(const char* ptr_name);

	static const char* GetCurrentEnumName(int value, const std::vector<std::string>& enumNames);
	static void SetEnumValues(const std::string& aux, ComponentScript* script);
public:
	/*--------------------INT-----------------------*/
	static void InspectorInputInt(int* ptr, const char* ptr_name);
	static void InspectorDragableInt(int* ptr, const char* ptr_name);
	static void InspectorSliderInt(int* ptr, const char* ptr_name, const int& min_value, const int& max_value);
	/*--------------------FLOAT-----------------------*/
	static void InspectorInputFloat(float* ptr, const char* ptr_name);
	static void InspectorDragableFloat(float* ptr, const char* ptr_name);
	static void InspectorSliderFloat(float* ptr, const char* ptr_name, const float& min_value, const float& max_value);
	/*--------------------BOOL-----------------------*/
	static void InspectorBool(bool* ptr, const char* ptr_name);
	/*--------------------STRING-----------------------*/
	static void InspectorString(std::string* ptr, const char* ptr_name);
	/*--------------------ENUM-----------------------*/
	static void InspectorEnum(int* ptr, const char* ptr_name, const char* enumAllString);
	/*--------------------PREFAB-----------------------*/
	static void InspectorPrefab(Prefab* ptr, const char* ptr_name);
	/*--------------------TEXT--------------------------*/
	static void InspectorText(const char* textToSHow);
	/*--------------------SEPARATOR--------------------------*/
	static void InspectorSeparator();
	/*--------------------TOOLTIP--------------------------*/
	static void InspectorToolTip(const char* textToSHow);
	/*--------------------SPACING--------------------------*/
	static void InspectorSpacing();
	/*--------------------GAMEOBJECT-----------------------*/
	static void InspectorGameObject(GameObject** ptr, const char* ptr_name);
	/*--------------------FUNCTION-----------------------*/
	static void ShowVoidFunction(std::function<void()> funct, const char* name);

private:

	std::map<std::string, std::function<void()>> functionMap;

	u64 resourceID = 0;

	std::vector<InspectorScriptData> inspector_variables;
	bool need_alien = false;
	std::string data_name;
	void* data_ptr = nullptr;
};

