#include "CollisionLayers.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Assimp/include/types.h"

bool CollisionLayers::GetIndexByName(string layer, int& index)
{
	for (int i = 0; i < names.size(); ++i) {
		if (names[i] == layer) {
			index = i;
			return true;
		}
	}
	return false;
}

bool CollisionLayers::GetNameByIndex(int index, string& name)
{
	if (index >= names.size() || index < 0) return false;
	name = names[index];
	return true;;
}

void CollisionLayers::AddLayer(std::string to_add)
{
	if (std::find(names.begin(), names.end(), to_add) != names.end())
		return;

	names.push_back(to_add);
	
	for (int i = 0; i < layers.size(); ++i)
		layers[i].Add(to_add);

	Layer new_layer(to_add);

	for (int i = 0; i < names.size(); ++i)
		new_layer.Add(names[i]);

	layers.push_back(new_layer);

	GenerateFastData();
}

void CollisionLayers::RemoveLayer(std::string to_remove)
{
	if (std::find(names.begin(), names.end(), to_remove) == names.end())
		return;

	vector<string>::iterator itr_names = names.begin();

	for (; itr_names != names.end(); ++itr_names) {
		if ((*itr_names) == to_remove) {
			names.erase(itr_names);
			break;
		}
	}

	vector<Layer>::iterator itr = layers.begin();

	for (; itr != layers.end(); ++itr) {
		if ((*itr).name == to_remove) {
			layers.erase(itr);
			break;
		}
	}

	for (int i = 0; i < layers.size() ; ++i)
		layers[i].Remove(to_remove);

	GenerateFastData();
}

void CollisionLayers::GenerateFastData()
{
	DeleteFastData();

	size = names.size();

	data = new bool* [size];
	for (int i = 0; i < size; i++)
		data[i] = new bool[size];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			data[i][j] = true;

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
			data[i][j] = layers[i].Get(names[j]);
	}
}

void CollisionLayers::DeleteFastData()
{
	if (data)
	{
		for (int i = 0; i < size; i++)
			delete[] data[i];

		delete[] data;
		data = nullptr;
	}
}

void CollisionLayers::SetLayers(int index_0, int index_1, bool to_change)
{
	bool value = !to_change;
	Layer* layer_0 = GetLayer(names[index_0]);
	Layer* layer_1 = GetLayer(names[index_1]);
	layer_0->Set(names[index_1], value);
	layer_1->Set(names[index_0], value);
	data[index_0][index_1] = value;
	data[index_1][index_0] = value;
}

void CollisionLayers::LoadLayers()
{
	if (App->file_system->Exists(FILE_PHYSICS))
	{

		JSON_Value* physics_value = json_parse_file(FILE_PHYSICS);
		JSON_Object* physics_object = json_value_get_object(physics_value);

		if (physics_value != nullptr && physics_object != nullptr)
		{
			JSONfilepack* physics_config = new JSONfilepack(FILE_PHYSICS, physics_object, physics_value);

			// Read All Layer Names -------------------------------

			JSONArraypack* layers_array = physics_config->GetArray("Layers");

			for (uint i = 0; i < layers_array->GetArraySize(); ++i) {
				names.push_back(layers_array->GetString("Layer"));
				layers_array->GetAnotherNode();
			}

			// Get Config for every Layer ------------------------

			int size = names.size();

			for (uint i = 0; i < size; ++i)
			{
				Layer layer(names[i]);

				for (uint j = 0; j < size; ++j)
					layer.Add(names[j], physics_config->GetBoolean((names[i] + "." + names[j]).c_str()));

				layers.push_back(layer);
			}

			delete physics_config;
		}
	}
	else
	{
		names.push_back("Default");
		names.push_back("Player");
		names.push_back("Wall");
		names.push_back("Ground");

		int size = names.size();

		for (uint i = 0; i < size; ++i)
		{
			Layer layer(names[i]);
			for (uint j = 0; j < size; ++j)
				layer.Add(names[j]);
			layers.push_back(layer);
		}
	}

	GenerateFastData();
}

void CollisionLayers::SaveLayers()
{
	remove(FILE_PHYSICS);

	JSON_Value* physics_value = json_value_init_object();
	JSON_Object* physics_object = json_value_get_object(physics_value);
	json_serialize_to_file_pretty(physics_value, FILE_PHYSICS);

	if (physics_value != nullptr && physics_object != nullptr)
	{
		JSONfilepack* physics_config = new JSONfilepack(FILE_PHYSICS, physics_object, physics_value);
		int size = names.size();

		physics_config->StartSave();

		JSONArraypack* layers_array = physics_config->InitNewArray("Layers");

		for (int i = 0; i < size; ++i)
		{
			layers_array->SetAnotherNode();
			layers_array->SetString("Layer", names[i].c_str());
		}

		for (uint i = 0; i < size; ++i)
			for (uint j = 0; j < size; ++j)
				physics_config->SetBoolean((names[i] + "." + names[j]).c_str(), data[i][j]);

		physics_config->FinishSave();

		delete physics_config;
	}
}

Layer* CollisionLayers::GetLayer(string name)
{
	vector<Layer>::iterator itr = layers.begin();

	for (; itr != layers.end(); ++itr) {
		if ((*itr).name == name) {
			return  &(*itr);
		}
	}
	return nullptr;
}

void Layer::Add(string layer, bool value)
{
	values.push_back(pair<string, bool>(layer, value));
}

void Layer::Remove(string layer)
{
	vector<pair<string, bool>>::iterator itr = values.begin();

	for (; itr != values.end(); ++ itr) {
		if ( (*itr).first == layer) {
			values.erase(itr);
			return;
		}
	}		
}

bool Layer::Get(string layer)
{
	vector<pair<string, bool>>::iterator itr = values.begin();

	for (; itr != values.end(); ++itr) {
		if ((*itr).first == layer) {
			return (*itr).second;
		}
	}
	return false;
}

void Layer::Set(string layer,bool value)
{
	vector<pair<string, bool>>::iterator itr = values.begin();

	for (; itr != values.end(); ++itr) {
		if ((*itr).first == layer) {
			(*itr).second = value;
			return;
		}
	}
			
}