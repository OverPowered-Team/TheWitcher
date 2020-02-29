#include "WwiseT.h"

#include "AK/SoundEngine/Common/AkMemoryMgr.h"                  // Memory Manager
#include "AK/SoundEngine/Common/AkModule.h"						// Default memory and stream managers
#include "AK/Win32/AkFilePackageLowLevelIOBlocking.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"                // Music Engine
#include "AK/Plugin/AkRoomVerbFXFactory.h"

// Wwise libs
#ifndef _DEBUG  // Profile build configuration must be loaded instead of Debug
#define AK_OPTIMIZED
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkRoomVerbFX.lib")
#else
#include "AK/Comm/AkCommunication.h"
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/CommunicationCentral.lib")
#pragma comment( lib, "AK/ws2_32.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkRoomVerbFX.lib")
#endif

#pragma comment( lib, "AK/dinput8.lib")
#pragma comment( lib, "AK/dsound.lib")
#pragma comment( lib, "AK/dxguid.lib")

#include <assert.h>
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "RandomHelper.h"
#include "mmgr/mmgr.h"


CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

// Wwise memory hooks
namespace AK
{
	void * AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}

	void FreeHook(void * in_ptr)
	{
		free(in_ptr);
	}

	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
	)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}
	void VirtualFreeHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
	)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
}

// Wwise stuff
bool WwiseT::InitSoundEngine()
{
	// Init mem manager
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		//Log("WwiseT.cpp",1,LogTypes::Error,"Could not create the memory manager.");
		return false;
	}

	// Init streaming manager
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not create the Streaming Manager");
		return false;
	}
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	// Init sound engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not initialize the Sound Engine.");
		return false;
	}

	//Init music engine
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not initialize the Music Engine.");
		return false;
	}

	// Initialize communications (not in release build!)
#ifndef AK_OPTIMIZED
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not initialize communication.");
		return false;
	}
#endif // AK_OPTIMIZED

	AKRESULT base_path_res = g_lowLevelIO.SetBasePath(AKTEXT("./Library/Audio/"));
	if (base_path_res != AK_Success)
	{
		assert(!"Invalid base path!");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Invalid base path!");
		return false;
	}

	// Load the init soundbank
	LoadBank("../../DLLs/Init.bnk");

	AKRESULT res = AK::StreamMgr::SetCurrentLanguage((AkOSChar*)"English");
	if (res == AK_Fail)
		assert(!"Invalid language!");

	// Init RTPCs
	AK::SoundEngine::SetRTPCValue("Pitch", 0, AK_INVALID_GAME_OBJECT);
	AK::SoundEngine::SetRTPCValue("PanLeft", 0, AK_INVALID_GAME_OBJECT);
	AK::SoundEngine::SetRTPCValue("PanRight", 0, AK_INVALID_GAME_OBJECT);
	AK::SoundEngine::SetRTPCValue("HighPassFilter", 0, AK_INVALID_GAME_OBJECT);
	AK::SoundEngine::SetRTPCValue("LowPassFilter", 0, AK_INVALID_GAME_OBJECT);
	AK::SoundEngine::SetState("swap_music", "state2");
	return true;
}

bool WwiseT::CloseSoundEngine()
{
	//Terminate comunication module (IMPORTANT: this must be the first module in being terminated)
#ifndef AK_OPTIMIZED
	AK::Comm::Term();
#endif 

	// Terminate the music engine
	AK::MusicEngine::Term();

	// Terminate the sound engine
	AK::SoundEngine::Term();

	// Terminate IO device
	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();

	return true;
}

void WwiseT::ProcessAudio()
{
	AK::SoundEngine::RenderAudio();
}

void WwiseT::LoadBank(const char * path)
{
	AkBankID bankID;
	AKRESULT eResult = AK::SoundEngine::LoadBank(path, AK_DEFAULT_POOL_ID, bankID);
	if (eResult != AK_Success)
	{
		assert(!"Could not initialize soundbank.");
	}
}

uint WwiseT::LoadBank(char* buffer, uint size)
{	
	AkBankID bankID = 0;

	AKRESULT eResult = AK::SoundEngine::LoadBank((void*)buffer, size, bankID);
	if (eResult != AK_Success)
	{
		assert(!"Could not initialize soundbank.");
	}

	return bankID;
}

bool WwiseT::UnLoadBank(const char* path)
{
	bool ret = true;
	AKRESULT res = AK::SoundEngine::UnloadBank(path, NULL);
	if (res != AK_Success)
		ret = false;

	return ret;
}

void WwiseT::UnLoadBank(uint bankID, char* buffer)
{
	AKRESULT eResult = AK::SoundEngine::UnloadBank(bankID, buffer);
}

void WwiseT::SetDefaultListener(uint id)
{
	AkGameObjectID tmp = id;
	AKRESULT eResult = AK::SoundEngine::SetDefaultListeners(&tmp, 1);
	if (eResult != AK_Success)
	{
		assert(!"Could not set GameObject as default listener.");
	}
}

WwiseT::AudioSource* WwiseT::CreateAudSource(const char * name)
{
	return new AudioSource(name);
}

WwiseT::AudioSource * WwiseT::CreateAudSource(uint id, const char * name)
{
	return new AudioSource(id, name);

}

void WwiseT::StopAllEvents()
{
	AK::SoundEngine::StopAll();
}

void WwiseT::PauseAll()
{
	AK::SoundEngine::PostEvent("Pause_All", AK_INVALID_GAME_OBJECT);
}

void WwiseT::ResumeAll()
{
	AK::SoundEngine::PostEvent("Resume_All", AK_INVALID_GAME_OBJECT);
}

WwiseT::AudioSource::AudioSource(const char* event_name)
{
	id = (u32)Random::GetRandomID();
	name.assign(event_name);
	AKRESULT eResult = AK::SoundEngine::RegisterGameObj(id, name.c_str());
	if (eResult != AK_Success)
	{
		assert(!"Could not register GameObject. See eResult variable to more info");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not register GameObject. See eResult variable to more info");
	}
}

WwiseT::AudioSource::AudioSource(uint pre_id, const char * event_name)
{
	id = pre_id;
	name.assign(event_name);
	AKRESULT eResult = AK::SoundEngine::RegisterGameObj(id, name.c_str());
	if (eResult != AK_Success)
	{
		assert(!"Could not register GameObject. See eResult variable to more info");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not register GameObject. See eResult variable to more info");
	}
}

WwiseT::AudioSource::~AudioSource()
{
	AKRESULT eResult = AK::SoundEngine::UnregisterGameObj(id);
	if (eResult != AK_Success)
	{
		assert(!"Could not unregister GameObject. See eResult variable to more info");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not unregister GameObject. See eResult variable to more info");
	}
}

void WwiseT::AudioSource::PlayEventByID(uint ev_id)
{
	AkUniqueID u_id = ev_id;
	AK::SoundEngine::PostEvent(u_id, id);
}

void WwiseT::AudioSource::PlayEventByName(const char * name)
{
	AK::SoundEngine::PostEvent(name, id);
}

void WwiseT::AudioSource::StopEventByName(const char * name)
{
	AK::SoundEngine::ExecuteActionOnEvent(name, AK::SoundEngine::AkActionOnEventType_Stop);
}

void WwiseT::AudioSource::PauseEventByName(const char * name)
{
	AK::SoundEngine::ExecuteActionOnEvent(name, AK::SoundEngine::AkActionOnEventType_Pause);
}

void WwiseT::AudioSource::ResumeEventByName(const char * name)
{
	AK::SoundEngine::ExecuteActionOnEvent(name, AK::SoundEngine::AkActionOnEventType_Resume);
}

void WwiseT::AudioSource::SetVolume(float new_volume)
{
	AK::SoundEngine::SetGameObjectOutputBusVolume(id, AK_INVALID_GAME_OBJECT, new_volume);
}

void WwiseT::AudioSource::SetPanLeft(float value)
{
	AK::SoundEngine::SetRTPCValue("PanLeft", value, id);
}

void WwiseT::AudioSource::SetPanRight(float value)
{
	AK::SoundEngine::SetRTPCValue("PanRight", value, id);
}

void WwiseT::AudioSource::SetMono()
{
	AkChannelConfig cfg;
	cfg.SetStandard(AK_SPEAKER_SETUP_MONO);
	AK::SoundEngine::SetBusConfig(id, cfg);
	cfg.Clear();
}

void WwiseT::AudioSource::SetStereo()
{
	AkChannelConfig cfg;
	cfg.SetStandard(AK_SPEAKER_SETUP_STEREO);
	AK::SoundEngine::SetBusConfig(id, cfg);
	cfg.Clear();
}

void WwiseT::AudioSource::SetPitch(float value) {
	//TODO Correct this
	AK::SoundEngine::SetRTPCValue("Pitch", value, id);
}

void WwiseT::AudioSource::SetListener(uint listener_id)
{
	AkGameObjectID tmp = listener_id;
	AKRESULT eResult = AK::SoundEngine::SetListeners(id, &tmp, 1);
	if (eResult != AK_Success)
	{
		assert(!"Could not set GameObject as listerner.");
		//Log("WwiseT.cpp", 1, LogTypes::Error, "Could not set GameObject as listerner.");
	}
}

uint WwiseT::AudioSource::GetID()const
{
	return id;
}

const char * WwiseT::AudioSource::GetName() const
{
	return name.c_str();
}

void WwiseT::AudioSource::SetSourcePos(float x, float y, float z, float x_front, float y_front, float z_front, float x_top, float y_top, float z_top)
{
	position.X = x;
	position.Y = y;
	position.Z = z;

	orient_front.X = x_front;
	orient_front.Y = y_front;
	orient_front.Z = z_front;
	orient_top.X = x_top;
	orient_top.Y = y_top;
	orient_top.Z = z_top;

	float length_front = sqrt(pow(orient_front.X, 2) + pow(orient_front.Y, 2) + pow(orient_front.Z, 2));
	float length_top = sqrt(pow(orient_top.X, 2) + pow(orient_top.Y, 2) + pow(orient_top.Z, 2));

	//Normalize vectors
	orient_front.X = orient_front.X / length_front;
	orient_front.Y = orient_front.Y / length_front;
	orient_front.Z = orient_front.Z / length_front;
	orient_top.X = orient_top.X / length_top;
	orient_top.Y = orient_top.Y / length_top;
	orient_top.Z = orient_top.Z / length_top;

	//Check if the are orthogonals
	float dot_prod = orient_top.X * orient_front.X + orient_top.Y * orient_front.Y + orient_top.Z * orient_front.Z;

	if (dot_prod >= 0.0001)
		assert(!"Vectors are not orthogonal!");

	AkSoundPosition sound_pos;
	sound_pos.Set(position, orient_front, orient_top);
	AKRESULT res = AK::SoundEngine::SetPosition((AkGameObjectID)id, sound_pos);
	if (res != AK_Success)
		assert(!"Something went wrong. Check the res variable for more info");
}

void WwiseT::AudioSource::SetListenerPos(float pos_x, float pos_y, float pos_z, float front_rot_x, float front_rot_y, float front_rot_z, float top_rot_x, float top_rot_y, float top_rot_z)
{
	
	// Setting position vectors
	math::float3 pos = { 0, 0, 0 };
	math::float3 rot_front = { 0, 0, 0 };
	math::float3 rot_top = { 0, 0, 0 };

	pos.x = pos_x;
	pos.y = pos_y;
	pos.z = pos_z;
	rot_front.x = front_rot_x;
	rot_front.y = front_rot_y;
	rot_front.z = front_rot_z;
	rot_top.x = top_rot_x;
	rot_top.y = top_rot_y;
	rot_top.z = top_rot_z;

	// Vectors must be normalized and not be orthogonals
	rot_front.Normalize();
	rot_top.Normalize();
	if (rot_front.x*rot_top.x + rot_front.y*rot_top.y + rot_front.z*rot_top.z >= 0.0001)
	{
		//Log("SET POSITION to Emmiter failed. Vectors are not orthogonal.", 1, LogTypes::Error, "");
		return;
	}

	// Convert float3 vectors to AkVectors
	AkVector ak_pos;
	AkVector ak_rot_front;
	AkVector ak_rot_top;

	ak_pos.X = pos.x;
	ak_pos.Y = pos.y;
	ak_pos.Z = pos.z;

	ak_rot_front.X = rot_front.x;
	ak_rot_front.Y = rot_front.y;
	ak_rot_front.Z = rot_front.z;

	ak_rot_top.X = rot_top.x;
	ak_rot_top.Y = rot_top.y;
	ak_rot_top.Z = rot_top.z;

	// Set position
	listener_pos.Set(ak_pos, ak_rot_front, ak_rot_top);
	AK::SoundEngine::SetPosition(id, listener_pos);
	
}

void WwiseT::AudioSource::ApplyEnvReverb(AkReal32 desired_level, const char * target)
{
	AkAuxSendValue environment;
	environment.listenerID = id;
	environment.fControlValue = desired_level;
	environment.auxBusID = AK::SoundEngine::GetIDFromString(target);
	AKRESULT res = AK::SoundEngine::SetGameObjectAuxSendValues(id, &environment, 2);
}

void WwiseT::AudioSource::ChangeState(const char * group_name, const char * new_state)
{
	AKRESULT res = AK::SoundEngine::SetState(group_name, new_state);
}
