#pragma once
#if WITH_FMOD
#include <fmod.hpp>
#else
namespace FMOD { class Sound; class Channel; }
#endif
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
	float mVolume = 0.5f;

	bool bLoop = false;
	bool bIsPlaying = false;
	bool bPaused = false;

	FMOD::Sound*   mFmodSound = nullptr;
	FMOD::Channel* mFmodChannel = nullptr;
};
