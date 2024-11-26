#pragma once
#include "Core/Entity/JObject.h"

class JAudioComponent : public JObject
{
public:
	JAudioComponent();
	explicit JAudioComponent(JTextView InName);
	~JAudioComponent() override = default;

public:
	void Play();
	void Stop();
	void Pause();

protected:
	float mVolume;

	bool bLoop;
	bool bIsPlaying;
};
