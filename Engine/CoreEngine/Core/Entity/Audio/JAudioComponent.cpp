#include "JAudioComponent.h"

#include "MSoundManager.h"

JAudioComponent::JAudioComponent()
	: mVolume(0.5f),
	  bLoop(false),
	  bIsPlaying(false),
	  bPaused(false),
	  mFmodSound(nullptr),
	  mFmodChannel(nullptr) {}

JAudioComponent::JAudioComponent(JTextView InName)
	: JActorComponent(InName)
{
	JAudioComponent::Initialize();
}

void JAudioComponent::Initialize()
{
	assert(
		   MSoundManager::Get().mFmodSystem->createSound(mPath.c_str(), FMOD_DEFAULT, nullptr, &mFmodSound)
		   ==
		   FMOD_OK,
		   "Sound creation failed");
}

void JAudioComponent::BeginPlay()
{
	JActorComponent::BeginPlay();
}

void JAudioComponent::Tick(float DeltaTime)
{
	JActorComponent::Tick(DeltaTime);
}

void JAudioComponent::Destroy()
{
	if (mFmodSound)
	{
		Stop();
		mFmodSound->release();
	}
}

void JAudioComponent::Play(bool bLoop)
{
	MSoundManager::Get().mFmodSystem->playSound(mFmodSound, nullptr, false, &mFmodChannel);

	mFmodChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
}

void JAudioComponent::Stop()
{
	if (mFmodChannel)
	{
		mFmodChannel->stop();
	}
}

void JAudioComponent::Pause()
{
	if (mFmodChannel)
	{
		mFmodChannel->isPlaying(&bIsPlaying);
		mFmodChannel->getPaused(&bPaused);
		if (bIsPlaying)
		{
			mFmodChannel->setPaused(!bPaused);
		}
	}
}
