#pragma once

enum class EventType
{
	ON_PLAY = 0,
	ON_PAUSE,
	ON_STOP,
	ON_GO_SELECT,
	ON_ASSET_SELECT,
	ON_ASSET_DELETE,
	ON_GO_DELETE,
	ON_UNLOAD_SCENE,
	//ON_UNPAUSE,
	//ON_DESTROY_OBJECT
	// Go nuts adding stuff
};