#pragma once

#include "Component.h"
#include "Globals.h"
#include "WwiseT.h"

class GameObject;

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(GameObject* parent);
	~ComponentAudioListener();
	void Update();
	void UpdateListenerPos();

	void Reset() override;

	void OnEnable() override;
	void OnDisable() override;

	void SaveComponent(JSONArraypack* to_save) override;
	void LoadComponent(JSONArraypack* to_load) override;

	bool DrawInspector() override;


public:
	WwiseT::AudioSource* listener = nullptr;
};
