#pragma once

/*
	Simple Audio Engine class by Darren Reid
	Wraps Wwise basic functionality.

	version 1.1 for Wwise 2017.2.2.6553


	add "C:\Audiokinetic\Wwise 2017.2.2.6553\SDK\include" and
	"C:\Audiokinetic\Wwise 2017.2.2.6553\SDK\samples\SoundEngine\Win32"
	and 
	"C:\Audiokinetic\Wwise 2017.2.2.6553\SDK\samples\SoundEngine\Common"
	to the include directories, and
	"C:\Audiokinetic\Wwise 2017.2.2.6553\SDK\Win32_vc140\Release(StaticCRT)\lib"
	or
	"C:\Audiokinetic\Wwise 2017.2.2.6553\SDK\Win32_vc140\Debug(StaticCRT)\lib"
	to the libs path for 32 bit builds, or the appropriate 64 bit paths for 64 bit builds.

	add the following cpps from the samples to the build:
	AkDefaultIOHookBlocking.cpp
	AkDefaultIOHookDeferred.cpp
	AkDefaultLowLevelIODispatcher.cpp
	AkFileLocationBase.cpp
	AkFilePackage.cpp
	AkFilePackageLUT.cpp

	Make sure "Treat Wchar_t as Built-in Type" is set to Yes (/Zc:wchar_t)
	*/

#ifdef NDEBUG
#define AK_OPTIMIZED
#endif

#include <string>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>                  // Memory Manager
#include <AK/SoundEngine/Common/AkModule.h>                     // Default memory and stream managers
#include <AK/SoundEngine/Common/IAkStreamMgr.h>                 // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>                    // Thread defines
#include <AkFilePackageLowLevelIOBlocking.h>                    // Sample low-level I/O implementation
#include <AK/SoundEngine/Common/AkSoundEngine.h>                // Sound engine

//only needed if we use interactive music
#include <AK/MusicEngine/Common/AkMusicEngine.h>                // Music Engine

// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED


class AudioEngine
{
	// We're using the default Low-Level I/O implementation that's part
	// of the SDK's sample code, with the file package extension
	CAkFilePackageLowLevelIOBlocking g_lowLevelIO;
public:
	

	bool Init();
	void ProcessAudio();
	void TermSoundEngine();
	void SetBasePath(std::string path);
	bool LoadBank(std::string bank);
	AkPlayingID PlayEvent(std::string eventName, AkGameObjectID gameObj);
	void StopEvent(std::string eventName, AkGameObjectID gameObjectID, 
		AkPlayingID playingID, AkTimeMs transitionDuration = 0);
	void PauseEvent(std::string eventName, AkGameObjectID gameObjectID,
		AkPlayingID playingID, AkTimeMs transitionDuration = 0);
	void ResumeEvent(std::string eventName, AkGameObjectID gameObjectID,
		AkPlayingID playingID, AkTimeMs transitionDuration = 0);
	void RegisterGameObject(AkGameObjectID gameObjectID);
	~AudioEngine();
};
