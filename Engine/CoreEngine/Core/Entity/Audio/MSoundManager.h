#pragma once
#include "JAudioComponent.h"
#include "Core/Manager/Manager_Base.h"

namespace FMOD
{
	class System;
}

class MSoundManager : public Manager_Base<JAudioComponent, MSoundManager>
{
public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	void Stop();
	void StopAll();
	void Pause();
	void PauseAll();

private:
	FMOD::System* mFmodSystem;

private:
	friend class TSingleton<MSoundManager>;
	friend class JAudioComponent;
	MSoundManager();
	~MSoundManager() = default;

public:
	MSoundManager(const MSoundManager&)            = delete;
	MSoundManager& operator=(const MSoundManager&) = delete;
};
