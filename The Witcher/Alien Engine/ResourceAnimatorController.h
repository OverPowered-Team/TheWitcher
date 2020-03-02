#pragma once

#include "Resource.h"
#include "Globals.h"
#include "NodeEditor/Include/imgui_node_editor.h"

#include <vector>

class ResourceAnimation;

class State
{
private:
	std::string name;
	float speed = 1.0;
	ResourceAnimation* clip = nullptr;

public:
	uint pin_in_id, pin_out_id, id;
	float time = 0;
	float fade_duration = 0, fade_time = 0;
	State* next_state = nullptr;

public:
	State();
	State(std::string name, ResourceAnimation* clip);

	void SetName(std::string name);
	void SetClip(ResourceAnimation* clip);
	void SetSpeed(float speed);

	float GetSpeed();
	std::string GetName();
	ResourceAnimation* GetClip();
};

class Condition {
public:
	std::string type = "";
	uint parameter_index;
	std::vector<std::string> comp_texts;
	std::string comp_text = "";

public:
	Condition(std::string type, std::string comp_text = "") { this->type = type; }
	virtual bool Compare(ResourceAnimatorController* controller) { return false; }

};

class IntCondition : public Condition {
public:
	int comp;

public:
	IntCondition(std::string type, uint index, int comp) : Condition(type) {
		this->comp = comp;
		this->parameter_index = index;
		comp_texts.push_back("Equal");
		comp_texts.push_back("Not Equal");
		comp_texts.push_back("Greater");
		comp_texts.push_back("Lesser");
		comp_text = "Greater";
	}
	bool Compare(ResourceAnimatorController* controller);
	void SetParameter(uint index) { this->parameter_index = index; }
	void SetCompValue(int comp) { this->comp = comp; }
	void SetCompText(std::string comp_text) { this->comp_text = comp_text; }

};

class FloatCondition : public Condition {
public:
	float comp;

public:
	FloatCondition(std::string type, uint index, float comp) : Condition(type) {
		this->comp = comp;
		this->parameter_index = index;
		comp_texts.push_back("Greater");
		comp_texts.push_back("Lesser");
		comp_text = "Greater";
	}
	bool Compare(ResourceAnimatorController* controller);
	void SetParameter(uint index) { this->parameter_index = index; }
	void SetCompValue(float comp) { this->comp = comp; }
	void SetCompText(std::string comp_text) { this->comp_text = comp_text; }
};

class BoolCondition : public Condition {

public:
	BoolCondition(std::string type, uint index) : Condition(type) {
		this->parameter_index = index;
		comp_texts.push_back("True");
		comp_texts.push_back("False");
		comp_text = "True";
	}
	bool Compare(ResourceAnimatorController* controller);
	void SetParameter(uint index) { this->parameter_index = index; }
	void SetCompText(std::string comp_text) { this->comp_text = comp_text; }
};


class Transition
{
private:
	State* source;
	State* target;
	std::vector<IntCondition*> int_conditions;
	std::vector<FloatCondition*> float_conditions;
	std::vector<BoolCondition*> bool_conditions;
	float blend = 2;

public:
	Transition();
	Transition(State* source, State* target, float blend);

public:
	void SetSource(State* source);
	void SetTarget(State* target);
	void SetBlend(float blend);

	State* GetSource();
	State* GetTarget();
	float GetBlend();

	//Handle Conditions
	void AddIntCondition();
	void AddIntCondition(std::string type, std::string comp_text, uint index, int comp);
	void AddFloatCondition();
	void AddFloatCondition(std::string type, std::string comp_text, uint index, float comp);
	void AddBoolCondition();
	void AddBoolCondition(std::string type, std::string comp_text, uint index);
	void RemoveIntCondition(IntCondition* int_condition);
	void RemoveFloatCondition(FloatCondition* float_condition);
	void RemoveBoolCondition(BoolCondition* bool_condition);
	std::vector<IntCondition*> GetIntConditions() { return int_conditions; }
	std::vector<FloatCondition*> GetFloatConditions() { return float_conditions; }
	std::vector<BoolCondition*> GetBoolConditions() { return bool_conditions; }
};

class ResourceAnimatorController : public Resource
{
private:
	State* current_state = nullptr;
	std::vector<State*> states;
	std::vector<Transition*> transitions;
	State* default_state = nullptr;

	std::vector <std::pair <std::string, int>> int_parameters;
	std::vector <std::pair <std::string, float>> float_parameters;
	std::vector <std::pair <std::string, bool>> bool_parameters;

private:
	ax::NodeEditor::EditorContext* ed_context = nullptr;

public:
	ResourceAnimatorController();
	~ResourceAnimatorController();

	int attached_references = 0;
	int id_bucket = 1;

	void ReImport(const u64& force_id = 0);
	//Parameters things

	//Getters / Setters
	const std::vector <std::pair <std::string, bool>>& GetBoolParameters();
	const std::vector <std::pair <std::string, float>>& GetFloatParameters();
	const std::vector <std::pair <std::string, int>>& GetIntParameters();
	void SetBoolParametersName(uint index, const std::string& name);
	void SetFloatParametersName(uint index, const std::string& name);
	void SetIntParametersName(uint index, const std::string& name);
	void SetBoolParametersValue(uint index, const bool& value);
	void SetFloatParametersValue(uint index, const float& value);
	void SetIntParametersValue(uint index, const int& value);

	void AddBoolParameter();
	void AddFloatParameter();
	void AddIntParameter();
	void AddBoolParameter(std::pair<std::string, bool> param); 
	void AddFloatParameter(std::pair<std::string, float> param);
	void AddIntParameter(std::pair<std::string, int> param);
	void RemoveBoolParameter(std::string name);
	void RemoveFloatParameter(std::string name);
	void RemoveIntParameter(std::string name);
	//string to const char for scripting
	void SetBool(std::string name, bool value);
	void SetFloat(std::string name, float value);
	void SetInt(std::string name, int value);

public:
    void FreeMemory();
	bool LoadMemory();
	bool ReadBaseInfo(const char* meta_file_path);
	void ReadLibrary(const char* meta_data);
	bool CreateMetaData(const u64& force_id = 0) override;
	bool DeleteMetaData() override;
	bool SaveAsset(const u64& force_id = 0);

	//AnimationHandle
	void Play();
	void Play(std::string state_name);
	void Update();
	void UpdateState(State* state);
	void Stop();
	bool CheckTriggers();


	//Transform
	bool GetTransform(std::string channel_name, float3& position, Quat& rotation, float3& scale);
	bool GetTransformState(State* state, std::string channel_name, float3& position, Quat& rotation, float3& scale);

	//States
	void AddState(std::string name, ResourceAnimation* clip, float speed = 1.0F);
	void RemoveState(std::string name);
	State* FindState(std::string name);
	State* FindState(uint id);
	State* FindStateFromPinId(uint pin_id);

	uint GetNumStates() const { return states.size(); }
	std::vector<State*> GetStates() { return states; }

	//Transitions
	void AddTransition(State* source, State* target, float blend);
	void RemoveTransition(std::string source_name, std::string target_name);
	std::vector<Transition*> GetTransitions() const { return transitions; }
	uint GetNumTransitions() const { return transitions.size(); }
	std::vector<Transition*> FindTransitionsFromSourceState(State* state);

	State* GetDefaultNode() const { return default_state; };
	void SetDefaultNode(State* state) { default_state = state; }
	ax::NodeEditor::EditorContext* GetEditorContext();
	std::string GetTypeString() const;

	std::string GetName();

	//void UnLoad();
	//void Load();
	void Reset();

	friend class Time;
};
