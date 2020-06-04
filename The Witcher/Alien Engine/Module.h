#pragma once

#include <string>
#include "JSONfilepack.h"

enum class EventType; 

class Module
{
private :
	bool enabled = true;

public:

	Module(bool start_enabled=true)
	{
		enabled = start_enabled;
	}

	virtual ~Module()
	{}

	virtual void LoadConfig(JSONfilepack*& config) {}
	virtual void SaveConfig(JSONfilepack*& config) {}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt) 
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt) 
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt) 
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
		
	virtual void HandleEvent(EventType eventType) {}

	virtual void PanelConfigOption() {};

	void SetEnable(bool enable)
	{
		enabled = enable;
	}

	bool IsEnabled()
	{
		return enabled;
	}

	std::string name;

};