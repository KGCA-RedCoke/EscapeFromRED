#pragma once
#include "common_include.h"

class JActor;
class XVertexMesh;
class JFont;
class Timer;
struct FBasicWindowData;

class Application final
{
public:
	void Run();

public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	[[nodiscard]] FORCEINLINE float_t GetDeltaSeconds() const { return mDeltaTime; }
	[[nodiscard]] FORCEINLINE int32_t GetFramePerSeconds() const { return mFramesPerSec; }
	[[nodiscard]] FORCEINLINE float GetCurrentTime() const { return mCurrentTime; }
	[[nodiscard]] uint32_t GetWindowWidth() const;
	[[nodiscard]] uint32_t GetWindowHeight() const;

private:
	void HandleFrame();
	void HandleTick();
	void CheckWindowClosure();
	void ResetValues();

public:
	static Application* s_AppInstance;

private:
	UPtr<class Window> mWindow;

	bool bRunning   = false;
	bool bMinimized = false;

	UPtr<Timer> mTimer;
	float_t     mDeltaTime    = 0;
	float_t     mTime         = 0;
	float_t     mCurrentTime  = 0;
	int32_t     mFramesPerSec = 0;
	int32_t     mFrameCounter = 0;

	UPtr<JFont> mFpsText = nullptr;

public:
	JArray<JActor*> Actors;

public:
	Application();
	Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData);
	~Application();
};
