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
#if WITH_FMOD
	assert(
		   MSoundManager::Get().mFmodSystem->createSound(mPath.c_str(), FMOD_DEFAULT, nullptr, &mFmodSound)
		   ==
		   FMOD_OK,
		   "Sound creation failed");
#endif
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
#if WITH_FMOD
	if (mFmodSound)
	{
		Stop();
		mFmodSound->release();
		mFmodSound = nullptr;
	}
#endif
}

void JAudioComponent::Play(bool bLoop)
{
#if WITH_FMOD
	MSoundManager::Get().mFmodSystem->playSound(mFmodSound, nullptr, false, &mFmodChannel);

	mFmodChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
#endif
}

void JAudioComponent::Stop()
{
#if WITH_FMOD
	if (mFmodChannel)
	{
		mFmodChannel->stop();
	}
#endif
}

void JAudioComponent::Pause()
{
#if WITH_FMOD
	if (mFmodChannel)
	{
		mFmodChannel->isPlaying(&bIsPlaying);
		mFmodChannel->getPaused(&bPaused);
		if (bIsPlaying)
		{
			mFmodChannel->setPaused(!bPaused);
		}
	}
#endif
}
