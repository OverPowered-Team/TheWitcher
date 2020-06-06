#pragma once

#include "Globals.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct Layer
{
public:

	Layer(string name): name(name){}

	string name;
	vector<pair<string, bool>> values;

	void Add(string layer, bool value_def = true);
	void Remove(string layer);
	bool Get(string layer);
	void Set(string layer, bool value);

};

class CollisionLayers
{
public :

	bool GetIndexByName(string name, int& index);
	bool GetNameByIndex(int index, string& name = string());

	void AddLayer(string layer);
	void RemoveLayer(string layer);
	void GenerateFastData();
	void DeleteFastData();
	void SetLayers(int layer_0, int layer_1, bool value);
	void LoadLayers();
	void SaveLayers();
	Layer* GetLayer(string name);

	uint			size = 0;
	bool**			data = nullptr;
	vector<string>  names;
	vector<Layer>	layers;

};