#ifndef WWISET_H_
#define WWISET_H_

#include <string>

#include "Globals.h"

#include "AK/SoundEngine/Common/AkTypes.h"

typedef uint32_t u32;
// Wwise memory hooks
namespace AK
{
#ifdef WIN32
	void * AllocHook(size_t in_size);
	void FreeHook(void * in_ptr);

	void * VirtualAllocHook(void * in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect);
	void VirtualFreeHook(void * in_pMemAddress, size_t in_size, DWORD in_dwFreeType);
#endif
}

// Wwise functions
namespace WwiseT
{
	class AudioSource
	{
	public:
		AudioSource(const char* name);
		AudioSource(uint id, const char* name);
		~AudioSource();

		// Events
		void PlayEventByID(uint id);
		void PlayEventByName(const char* name);
		void StopEventByName(const char* name);
		void PauseEventByName(const char* name);
		void ResumeEventByName(const char* name);
		void ApplyEnvReverb(AkReal32 desired_level, const char * target);
		void ChangeState(const char* group_name, const char* new_state);
		// Seters
		void SetVolume(float new_volume);
		void SetPanLeft(float value);
		void SetPanRight(float value);
		void SetMono();
		void SetStereo();
		void SetPitch(float value);
		void SetListener(uint listener_id);
		void SetSourcePos(float x, float y, float z, float x_front, float y_front, float z_front, float x_top, float y_top, float z_top);
		void SetListenerPos(float pos_x, float pos_y, float pos_z, float front_rot_x, float front_rot_y, float front_rot_z, float top_rot_x, float top_rot_y, float top_rot_z);
		void SetSwitch(AkGameObjectID game_object_id, const char* switch_group_id, const char* switch_state_id);
		// Geters
		unsigned int GetID() const;
		const char* GetName() const;
		AkUInt32 GetWwiseIDFromString(const char* Wwise_name);

	private:
		u32 id;
		std::string name; // Event name
		AkSoundPosition source_pos;
		AkListenerPosition listener_pos;
		AkVector position;
		AkVector orient_top;
		AkVector orient_front;
	};

	bool InitSoundEngine();
	bool CloseSoundEngine(); // Close wwise
	void ProcessAudio();
	void LoadBank(const char* path);
	uint LoadBank(char* buffer, uint size);
	bool UnLoadBank(const char* path);
	void UnLoadBank(uint bankID, char* buffer);

	void SetDefaultListener(uint id);

	// Utils
	AudioSource* CreateAudSource(const char* name);
	AudioSource* CreateAudSource(uint id, const char* name);
	void StopAllEvents();
	void PauseAll();
	void ResumeAll();
}
// Make wwise methods here :)
// We will access all the functions in the engine from here

#endif