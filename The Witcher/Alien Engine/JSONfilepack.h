#pragma once

#include "Parson/parson.h"
#include <string>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "SDL/include/SDL_scancode.h"

class Color;
class JSONArraypack;

class __declspec(dllexport) JSONfilepack {

public:

	JSONfilepack(const char* path, JSON_Object*& object, JSON_Value*& value);

	~JSONfilepack();

	static JSONfilepack* CreateJSON(const char* path);
	static JSONfilepack* GetJSON(const char* path);
	static void FreeJSON(JSONfilepack* json);

	void StartSave();
	void FinishSave();
	
	void SetNumber(const char* name, const double& number);
	double GetNumber(const char* name);

	void SetBoolean(const char* name, const bool& boolean);
	bool GetBoolean(const char* name);

	void SetColor(const char* name, const Color& color);
	Color GetColor(const char* name);

	void SetFloat3(const char* name, const float3& numbers);
	float3 GetFloat3(const char* name);

	void SetFloat4(const char* name, const float4& numbers);
	float4 GetFloat4(const char* name);

	void SetQuat(const char* name, const Quat& numbers);
	Quat GetQuat(const char* name);
	
	void SetNumberArray(const char* name, float* numbers, uint size);
	float* GetNumberArray(const char* name);

	void SetUintArray(const char* name, uint* numbers, uint size);
	uint* GetUintArray(const char* name);

	void SetShortcutCodes(const char* name, SDL_Scancode* codes);
	SDL_Scancode* GetShortcutCodes(const char* name);

	void SetArrayString(const char* name, std::string* strings, uint size);
	void SetArrayNumber(const char* name, const double& number);
	double GetArrayNumber(const char* name, const uint& index);
	std::string* GetArrayString(const char* name);

	void SetString(const char* name, const char* string_parameter);
	const char* GetString(const char* name);

	JSONArraypack* InitNewArray(const char* name);

	JSONArraypack* GetArray(const char* name);

	void ClearArrays();

private:

	const char* path;
	JSON_Object* object = nullptr;
	JSON_Value* value = nullptr;
	JSON_Value* save_value = nullptr;
	JSON_Object* save_object = nullptr;

	std::vector<JSONArraypack*> arrays;

};

class __declspec(dllexport) JSONArraypack {

public:

	JSONArraypack(JSON_Array* arr, JSON_Value* value) { this->arr = arr; this->value = value; }
	~JSONArraypack();

	void SetNumber(const char* name, const double& number);
	double GetNumber(const char* name);

	void SetBoolean(const char* name, const bool& boolean);
	bool GetBoolean(const char* name);

	void SetColor(const char* name, const Color& color);
	Color GetColor(const char* name);

	void SetFloat3(const char* name, const float3& numbers);
	float3 GetFloat3(const char* name);

	void SetFloat4(const char* name, const float4& numbers);
	float4 GetFloat4(const char* name);

	void SetQuat(const char* name, const Quat& numbers);
	Quat GetQuat(const char* name);

	void SetAnotherNode();
	bool GetAnotherNode();
	void GetFirstNode();
	void GetNode(const uint& index);

	uint GetArraySize();

	void SetString(const char* name, const char* string_parameter);

	const char* GetString(const char* name);

	JSONArraypack* InitNewArray(const char* name);

	JSONArraypack* GetArray(const char* name);


private:

	JSON_Array* arr = nullptr;
	JSON_Value* value = nullptr;

	std::vector<JSONArraypack*> arrays;
	uint index = 0;
};