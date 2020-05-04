#include "JSONfilepack.h"
#include "Color.h"
#include "mmgr/mmgr.h"

JSONfilepack::JSONfilepack(const char* path, JSON_Object*& object, JSON_Value*& value)
	: path(path), object(object), value(value)
{
}

JSONfilepack::~JSONfilepack()
{
	if (!arrays.empty()) {
		std::vector<JSONArraypack*>::iterator item = arrays.begin();
		for (; item != arrays.end(); ++item) {
			delete* item;
		}
		arrays.clear();
	}

	if (value != nullptr)
		json_value_free(value);
}

JSONfilepack* JSONfilepack::CreateJSON(const char* path)
{
	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);
 
	json_serialize_to_file_pretty(value, path);

	return new JSONfilepack(path, json_object, value);
}

JSONfilepack* JSONfilepack::GetJSON(const char* path)
{
	JSON_Value* value = json_parse_file(path);
	JSON_Object* object = json_value_get_object(value);

	return new JSONfilepack(path, object, value);
}
void JSONfilepack::FreeJSON(JSONfilepack* json)
{
	if (json != nullptr) {
		delete json;
	}
}
void JSONfilepack::StartSave()
{
	save_value = json_parse_file(path);
	save_object = json_object(save_value);
}

void JSONfilepack::FinishSave()
{
	json_serialize_to_file_pretty(save_value, path);
	json_value_free(save_value);
	save_value = nullptr;
	save_object = nullptr;
}

void JSONfilepack::SetNumber(const char* name, const double& number)
{
	json_object_dotset_number(save_object, name, number);
}

double JSONfilepack::GetNumber(const char* name, double default_value)
{
	try {
		return json_object_dotget_number(object, name);
	}
	catch (...) {
		return default_value;
	}
}

void JSONfilepack::SetBoolean(const char* name, const bool& boolean)
{
	json_object_dotset_boolean(save_object, name, boolean);
}

bool JSONfilepack::GetBoolean(const char* name, bool default_value)
{
	try {
		return json_object_dotget_boolean(object, name);
	}
	catch (...) {
		return default_value;
	}
}

void JSONfilepack::SetColor(const char* name, const Color& color)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, color.r);
	json_array_append_number(arr, color.g);
	json_array_append_number(arr, color.b);
	json_array_append_number(arr, color.a);
}

Color JSONfilepack::GetColor(const char* name, Color default_color)
{
	try {
		JSON_Array* arr = json_object_dotget_array(object, name);

		Color color;
		color.r = json_array_get_number(arr, 0);
		color.g = json_array_get_number(arr, 1);
		color.b = json_array_get_number(arr, 2);
		color.a = json_array_get_number(arr, 3);

		return color;
	}
	catch (...) {
		return default_color;
	}
}

void JSONfilepack::SetFloat3(const char* name, const float3& numbers)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
}

float3 JSONfilepack::GetFloat3(const char* name, const float3& default_float3)
{
	try {
		JSON_Array* arr = json_object_dotget_array(object, name);

		float3 numbers;
		numbers.x = json_array_get_number(arr, 0);
		numbers.y = json_array_get_number(arr, 1);
		numbers.z = json_array_get_number(arr, 2);

		return numbers;
	}
	catch (...) {
		return default_float3;
	}
}

void JSONfilepack::SetFloat4(const char* name, const float4& numbers)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
	json_array_append_number(arr, numbers.w);
}

float4 JSONfilepack::GetFloat4(const char* name, const float4& default_float4)
{
	try {
		JSON_Array* arr = json_object_dotget_array(object, name);

		float4 numbers;
		numbers.x = json_array_get_number(arr, 0);
		numbers.y = json_array_get_number(arr, 1);
		numbers.z = json_array_get_number(arr, 2);
		numbers.w = json_array_get_number(arr, 3);

		return numbers;
	}
	catch (...) {
		return default_float4;
	}
}

void JSONfilepack::SetQuat(const char* name, const Quat& numbers)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
	json_array_append_number(arr, numbers.w);
}

Quat JSONfilepack::GetQuat(const char* name, const Quat& default_quat)
{
	try {
		JSON_Array* arr = json_object_dotget_array(object, name);

		Quat quat;
		quat.x = json_array_get_number(arr, 0);
		quat.y = json_array_get_number(arr, 1);
		quat.z = json_array_get_number(arr, 2);
		quat.w = json_array_get_number(arr, 3);

		return quat;
	}
	catch (...) {
		return default_quat;
	}
}

void JSONfilepack::SetNumberArray(const char* name, float* numbers, uint size)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	for (uint i = 0; i < size; ++i) {
		json_array_append_number(arr, numbers[i]);
	}
}

float* JSONfilepack::GetNumberArray(const char* name)
{
	JSON_Array* arr = json_object_dotget_array(object, name);
	
	uint size = json_array_get_count(arr);

	float* numbers = new float[size];
	
	for (uint i = 0; i < size; ++i) {
		numbers[i] = json_array_get_number(arr, i);
	}
	
	return numbers;
}

void JSONfilepack::SetUintArray(const char* name, uint* numbers, uint size)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	for (uint i = 0; i < size; ++i) {
		json_array_append_number(arr, numbers[i]);
	}
}

uint* JSONfilepack::GetUintArray(const char* name)
{
	JSON_Array* arr = json_object_dotget_array(object, name);

	uint size = json_array_get_count(arr);

	uint* numbers = new uint[size];

	for (uint i = 0; i < size; ++i) {
		numbers[i] = json_array_get_number(arr, i);
	}

	return numbers;
}

void JSONfilepack::SetShortcutCodes(const char* name, SDL_Scancode* codes)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	for (uint i = 0; i < 3; ++i) {
		json_array_append_number(arr, codes[i]);
	}
}

SDL_Scancode* JSONfilepack::GetShortcutCodes(const char* name)
{
	JSON_Array* arr = json_object_dotget_array(object, name);

	uint size = json_array_get_count(arr);

	SDL_Scancode codes[3] = { SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN };

	for (uint i = 0; i < 3; ++i) {
		codes[i] = static_cast<SDL_Scancode>(static_cast<int>(json_array_get_number(arr, i)));
	}

	return codes;
}

void JSONfilepack::SetArrayString(const char* name, std::string* strings, uint size)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(save_object, name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	for (uint i = 0; i < size; ++i) {
		json_array_append_string(arr, strings[i].data());
	}
}

void JSONfilepack::SetArrayNumber(const char* name, const double& number)
{
	JSON_Array* arr = json_object_dotget_array(save_object, name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);

		json_object_dotset_value(save_object, name, new_val);
	}
	json_array_append_number(arr, number);
}

double JSONfilepack::GetArrayNumber(const char* name, const uint& index)
{
	JSON_Array* arr = json_object_dotget_array(object, name);
	return json_array_get_number(arr, index);
}


std::string* JSONfilepack::GetArrayString(const char* name)
{
	JSON_Array* arr = json_object_dotget_array(object, name);

	uint size = json_array_get_count(arr);

	std::string* strings = new std::string[size];

	for (uint i = 0; i < size; ++i) {
		strings[i] = json_array_get_string(arr, i);
	}

	return strings;
}

void JSONfilepack::SetString(const char* name, const char* string_parameter)
{
	json_object_dotset_string(save_object, name, string_parameter);
}

const char* JSONfilepack::GetString(const char* name, const std::string& default_string)
{
	try {
		return json_object_dotget_string(object, name);
	}
	catch (...) {
		return default_string.data();
	}
}

JSONArraypack* JSONfilepack::InitNewArray(const char* name)
{
	JSON_Value* val = json_value_init_array();
	json_object_dotset_value(save_object, name, val);

	JSONArraypack* array_pack = new JSONArraypack(json_value_get_array(val), json_value_init_object());
	arrays.push_back(array_pack);

	return array_pack;
}

JSONArraypack* JSONfilepack::GetArray(const char* name)
{
	JSON_Array* arr = nullptr;
	if (save_object != nullptr) {
		arr = json_object_dotget_array(save_object, name);
	}
	else {
		arr = json_object_dotget_array(object, name);
	}
	if (arr != nullptr) {
		JSON_Value* value = json_array_get_value(arr, 0);
		if (value != nullptr) {
			JSONArraypack* array_pack = new JSONArraypack(arr, value);
			arrays.push_back(array_pack);
			return array_pack;
		}
		return nullptr;
	}
	return nullptr;
}

void JSONfilepack::ClearArrays()
{
	arrays.clear();
}

JSONArraypack::~JSONArraypack()
{
	if (!arrays.empty()) {
		std::vector<JSONArraypack*>::iterator item = arrays.begin();
		for (; item != arrays.end(); ++item) {
			delete* item;
		}
		arrays.clear();
	}
}

void JSONArraypack::SetNumber(const char* name, const double& number)
{
	json_object_dotset_number(json_value_get_object(value), name, number);
	json_array_append_value(arr, value);
}

double JSONArraypack::GetNumber(const char* name, double default_value)
{
	try {
		return json_object_dotget_number(json_value_get_object(value), name);
	}
	catch (...) {
		return default_value;
	}
}

void JSONArraypack::SetBoolean(const char* name, const bool& boolean)
{
	json_object_dotset_boolean(json_value_get_object(value), name, boolean);
}

bool JSONArraypack::GetBoolean(const char* name, bool default_value)
{
	try {
		return json_object_dotget_boolean(json_value_get_object(value), name);
	}
	catch (...) {
		return default_value;
	}
}

void JSONArraypack::SetColor(const char* name, const Color& color)
{
	JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(json_value_get_object(value), name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, color.r);
	json_array_append_number(arr, color.g);
	json_array_append_number(arr, color.b);
	json_array_append_number(arr, color.a);
}

Color JSONArraypack::GetColor(const char* name, Color default_color)
{
	try {
		JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);

		Color color;
		color.r = json_array_get_number(arr, 0);
		color.g = json_array_get_number(arr, 1);
		color.b = json_array_get_number(arr, 2);
		color.a = json_array_get_number(arr, 3);

		return color;
	}
	catch (...) {
		return default_color;
	}
}

void JSONArraypack::SetFloat3(const char* name, const float3& numbers)
{
	JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(json_value_get_object(value), name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
}

float3 JSONArraypack::GetFloat3(const char* name, const float3& default_float3)
{
	try {
		JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);

		float3 numbers;
		numbers.x = json_array_get_number(arr, 0);
		numbers.y = json_array_get_number(arr, 1);
		numbers.z = json_array_get_number(arr, 2);

		return numbers;
	}
	catch (...) {
		return default_float3;
	}
}

void JSONArraypack::SetFloat4(const char* name, const float4& numbers)
{
	JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(json_value_get_object(value), name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
	json_array_append_number(arr, numbers.w);
}

float4 JSONArraypack::GetFloat4(const char* name, const float4& default_float4)
{
	try {
		JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);

		float4 numbers;
		numbers.x = json_array_get_number(arr, 0);
		numbers.y = json_array_get_number(arr, 1);
		numbers.z = json_array_get_number(arr, 2);
		numbers.w = json_array_get_number(arr, 3);

		return numbers;
	}
	catch (...) {
		return default_float4;
	}
}

void JSONArraypack::SetQuat(const char* name, const Quat& numbers)
{
	JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);
	if (arr == nullptr) {
		JSON_Value* new_val = json_value_init_array();
		arr = json_value_get_array(new_val);
		json_object_dotset_value(json_value_get_object(value), name, new_val);
	}
	else {
		json_array_clear(arr);
	}
	json_array_append_number(arr, numbers.x);
	json_array_append_number(arr, numbers.y);
	json_array_append_number(arr, numbers.z);
	json_array_append_number(arr, numbers.w);
}

Quat JSONArraypack::GetQuat(const char* name, const Quat& default_quat)
{
	try {
		JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);

		Quat quat;
		quat.x = json_array_get_number(arr, 0);
		quat.y = json_array_get_number(arr, 1);
		quat.z = json_array_get_number(arr, 2);
		quat.w = json_array_get_number(arr, 3);

		return quat;
	}
	catch (...) {
		return default_quat;
	}
}

void JSONArraypack::SetAnotherNode()
{
	// I hope when destroying the core value of the file everything is deleted :) 
	value = json_value_init_object();
	json_array_append_value(arr, value);
}

bool JSONArraypack::GetAnotherNode()
{
	++index;
	if (index < json_array_get_count(arr)) {
		value = json_array_get_value(arr, index);
		return true;
	}
	else {
		return false;
	}
}

void JSONArraypack::GetFirstNode()
{
	index = 0;
	value = json_array_get_value(arr, index);
}

void JSONArraypack::GetNode(const uint& index)
{
	this->index = index;
	value = json_array_get_value(arr, index);
}

uint JSONArraypack::GetArraySize()
{
	return json_array_get_count(arr);
}

void JSONArraypack::SetString(const char* name, const char* string_parameter)
{
	json_object_dotset_string(json_value_get_object(value), name, string_parameter);
}

const char* JSONArraypack::GetString(const char* name, std::string default_string)
{
	try {
		const char* ret = json_object_dotget_string(json_value_get_object(value), name);
		return ret;
	}
	catch (...) {
		return default_string.data();
	}
}

JSONArraypack* JSONArraypack::InitNewArray(const char* name)
{
	JSON_Value* val = json_value_init_array();
	json_object_dotset_value(json_value_get_object(value), name, val);

	JSONArraypack* array_pack = new JSONArraypack(json_value_get_array(val), json_value_init_object());
	arrays.push_back(array_pack);

	return array_pack;
}

JSONArraypack* JSONArraypack::GetArray(const char* name)
{
	JSON_Array* arr = json_object_dotget_array(json_value_get_object(value), name);
	JSON_Value* value = json_array_get_value(arr, 0);
	JSONArraypack* array_pack = new JSONArraypack(arr, value);
	arrays.push_back(array_pack);

	return array_pack;
}
