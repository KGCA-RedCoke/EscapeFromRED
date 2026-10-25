#include "MSoundManager.h"
#if WITH_FMOD
#include <fmod.hpp>
#include <fmod_errors.h>
#endif

MSoundManager::MSoundManager()
{
	MSoundManager::Initialize();
}


void MSoundManager::Initialize()
{
#if WITH_FMOD
	FMOD_RESULT result;

	// Creates an instance of the FMOD system.
	result = System_Create(&mFmodSystem);

	assert(result == FMOD_OK, "fmod system creation failed");

	// Initialize the system object and prepare FMOD for playback.
	result = mFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	assert(result == FMOD_OK, "fmod system initialize failed");
#else
	LOG_CORE_INFO("Audio disabled: build with EnableFmod=true to enable the FMOD backend.");
#endif
}

void MSoundManager::Update(float DeltaTime)
{
#if WITH_FMOD
	if (mFmodSystem) mFmodSystem->update();
#endif
}

void MSoundManager::Render() {}

void MSoundManager::Release()
{
#if WITH_FMOD
	if (mFmodSystem)
	{
		mFmodSystem->close();
		mFmodSystem->release();
		mFmodSystem = nullptr;
	}
#endif
}

void MSoundManager::Stop() {}

void MSoundManager::StopAll()
{
	for (auto& list : mManagedList)
	{
		list.second->Stop();
	}
}

void MSoundManager::Pause() {}

void MSoundManager::PauseAll()
{
	for (auto& list : mManagedList)
	{
		list.second->Pause();
	}
}
