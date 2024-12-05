#pragma once
#include <fmod.hpp>
#include "Core/Entity/Component/JActorComponent.h"

class JAudioComponent : public JActorComponent
{
public:
	JAudioComponent();
	JAudioComponent(JTextView InName);
	~JAudioComponent() override = default;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	void Play(bool bLoop = false);
	void Stop();
	void Pause();

protected:
	float mVolume;

	bool bLoop;
	bool bIsPlaying;
	bool  bPaused;

	FMOD::Sound*   mFmodSound;
	FMOD::Channel* mFmodChannel;
};
