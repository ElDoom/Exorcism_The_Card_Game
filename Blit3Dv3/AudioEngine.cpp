#include "AudioEngine.h"
#include <stdlib.h>

//below needed for VirtualAlloc etc
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cassert>

//add the following libraries to the build
#pragma comment(lib, "AkSoundEngine.lib") //sound engine core
#pragma comment(lib, "AkMemoryMgr.lib") //memory manager
#pragma comment(lib, "AkStreamMgr.lib") //io manager
#pragma comment(lib, "AkMusicEngine.lib") //interactive dynamic music
#ifndef AK_OPTIMIZED
#pragma comment(lib, "CommunicationCentral.lib") //debug comms
#endif

//source plugin libs
#pragma comment(lib, "AkAudioInputSource.lib")
#pragma comment(lib, "AkSilenceSource.lib")
#pragma comment(lib, "AkSineSource.lib")
#pragma comment(lib, "AkSynthOneSource.lib")
#pragma comment(lib, "AkToneSource.lib")

//effect plugins
#pragma comment(lib, "AkCompressorFX.lib")
#pragma comment(lib, "AkDelayFX.lib")
#pragma comment(lib, "AkExpanderFX.lib")
#pragma comment(lib, "AkFlangerFX.lib")
#pragma comment(lib, "AkGainFX.lib")
#pragma comment(lib, "AkGuitarDistortionFX.lib")
#pragma comment(lib, "AkHarmonizerFX.lib")
#pragma comment(lib, "AkMatrixReverbFX.lib")
#pragma comment(lib, "AkMeterFX.lib")
#pragma comment(lib, "AkParametricEQFX.lib")
#pragma comment(lib, "AkPeakLimiterFX.lib")
#pragma comment(lib, "AkPitchShifterFX.lib")
#pragma comment(lib, "AkRoomVerbFX.lib")
#pragma comment(lib, "AkStereoDelayFX.lib")
#pragma comment(lib, "AkTimeStretchFX.lib")
#pragma comment(lib, "AkTremoloFX.lib")

//codec plugins
#pragma comment(lib, "AkVorbisDecoder.lib")

//motion plugins
#pragma comment(lib, "AkMotionGeneratorSource.lib")
//#pragma comment(lib, "AkRumble.lib")

//platform-specific (windows)
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dxguid.lib")
#ifndef AK_OPTIMIZED
#pragma comment(lib, "Ws2_32.lib")
#endif

// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}
	void FreeHook(void * in_ptr)
	{
		free(in_ptr);
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
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
#endif
}

bool AudioEngine::Init()
{
	//
	// Create and initialize an instance of the default memory manager. Note
	// that you can override the default memory manager with your own. Refer
	// to the SDK documentation for more information.
	//

	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}

	//
	// Create and initialize an instance of the default streaming manager. Note
	// that you can override the default streaming manager with your own. Refer
	// to the SDK documentation for more information.
	//

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}

	//
	// Create a streaming device with blocking low-level I/O handshaking.
	// Note that you can override the default low-level I/O module with your own. Refer
	// to the SDK documentation for more information.        
	//
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	// Customize the streaming device settings here.

	// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.
	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	//
	// Create the Sound Engine
	// Using default initialization parameters
	//

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}

	//NEXT SECTION ONLY NEEDED IF USING INTERACTIVE MUSIC

	//
	// Initialize the music engine
	// Using default initialization parameters
	//

	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}

#ifndef AK_OPTIMIZED
	//
	// Initialize communications (not in release build!)
	//
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}
#endif // AK_OPTIMIZED

	return true;
}

void AudioEngine::ProcessAudio()
{
	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();
}

void AudioEngine::TermSoundEngine()
{
#ifndef AK_OPTIMIZED
	//
	// Terminate Communication Services
	//
	AK::Comm::Term();
#endif // AK_OPTIMIZED

	//
	// Terminate the music engine
	//

	AK::MusicEngine::Term();

	//
	// Terminate the sound engine
	//

	AK::SoundEngine::Term();

	// Terminate the streaming device and streaming manager

	// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
	g_lowLevelIO.Term();

	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();
}


//conversion for strings to wide chars
inline std::wstring convert(const std::string& as)
{
	wchar_t* buf = new wchar_t[as.size() * 2 + 2];
	swprintf(buf, as.size() * 2 + 2, L"%S", as.c_str());
	std::wstring rval = buf;
	delete[] buf;
	return rval;
}

void AudioEngine::SetBasePath(std::string path)
{
	g_lowLevelIO.SetBasePath(convert(path).c_str());
	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
}

bool AudioEngine::LoadBank(std::string bank)
{
	AkBankID bankID; // Not used. These banks can be unloaded with their file name.
	AKRESULT eResult = AK::SoundEngine::LoadBank(convert(bank).c_str(), AK_DEFAULT_POOL_ID, bankID);
	assert(eResult == AK_Success);
	return(eResult == AK_Success);
}

AkPlayingID AudioEngine::PlayEvent(std::string eventName, AkGameObjectID gameObj)
{
	AkPlayingID playingID = AK::SoundEngine::PostEvent(
		convert(eventName).c_str(),                   // Name of the event (not case sensitive).
		gameObj                             // Associated game object ID
		);

	return playingID;
}

void AudioEngine::StopEvent(std::string eventName, AkGameObjectID gameObjectID,
	AkPlayingID playingID, AkTimeMs transitionDuration)
{
	AK::SoundEngine::ExecuteActionOnEvent(convert(eventName).c_str(),
		AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop,
		gameObjectID, transitionDuration, AkCurveInterpolation_Linear,
		playingID);
}

void AudioEngine::PauseEvent(std::string eventName, AkGameObjectID gameObjectID,
	AkPlayingID playingID, AkTimeMs transitionDuration)
{
	AK::SoundEngine::ExecuteActionOnEvent(convert(eventName).c_str(),
		AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause,
		gameObjectID, transitionDuration, AkCurveInterpolation_Linear,
		playingID);
}

void AudioEngine::ResumeEvent(std::string eventName, AkGameObjectID gameObjectID,
	AkPlayingID playingID, AkTimeMs transitionDuration)
{
	AK::SoundEngine::ExecuteActionOnEvent(convert(eventName).c_str(),
		AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume,
		gameObjectID, transitionDuration, AkCurveInterpolation_Linear,
		playingID);
}

AudioEngine::~AudioEngine()
{
	TermSoundEngine();
}

void AudioEngine::RegisterGameObject(AkGameObjectID gameObjectID)
{
	AK::SoundEngine::RegisterGameObj(gameObjectID);
	AK::SoundEngine::SetDefaultListeners(&gameObjectID, 1);
}

