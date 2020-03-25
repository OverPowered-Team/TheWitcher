#pragma once

class __declspec(dllexport) SceneManager {
public:
	static void LoadScene(const char* scene_name);
	static int ScenesLoaded();

private:

	static int scenes_loaded;
};
