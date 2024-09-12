#pragma once
#include "JCamera.h"
#include "Core/Manager/Manager_Base.h"

class MCameraManager : public Manager_Base<JCamera, MCameraManager>
{
public:
	void Initialize();

	FORCEINLINE JCamera* GetCurrentMainCam() const { return mCurrentCamera; }
	FORCEINLINE JCamera* GetCam(JWTextView InName) { return FetchResource<JCamera>(InName.data()); }

	void SetCurrentMainCam(JWTextView InName);

private:
	JCamera* mCurrentCamera;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MCameraManager>;
	friend class MManagerInterface;

	MCameraManager() = default;

public:
	MCameraManager(const MCameraManager&)            = delete;
	MCameraManager& operator=(const MCameraManager&) = delete;

#pragma endregion
};
