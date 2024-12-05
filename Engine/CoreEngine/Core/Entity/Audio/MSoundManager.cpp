#include "MSoundManager.h"
#include <fmod.hpp>
#include <fmod_errors.h>

MSoundManager::MSoundManager()
{
	MSoundManager::Initialize();
}


void MSoundManager::Initialize()
{
	FMOD_RESULT result;

	// Creates an instance of the FMOD system.
	result = System_Create(&mFmodSystem);

	assert(result == FMOD_OK, "fmod system creation failed");

	// Initialize the system object and prepare FMOD for playback.
	result = mFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	assert(result == FMOD_OK, "fmod system initialize failed");
}

void MSoundManager::Update(float DeltaTime)
{
	mFmodSystem->update();
}

void MSoundManager::Render() {}

void MSoundManager::Release()
{
	if (mFmodSystem)
	{
		mFmodSystem->close();
		mFmodSystem->release();
		mFmodSystem = nullptr;
	}
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
