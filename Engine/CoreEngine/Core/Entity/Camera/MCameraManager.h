#pragma once
#include "JCamera.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/Manager_Base.h"

class MCameraManager : public Manager_Base<JCamera, MCameraManager>
{
private:
	void Initialize_Internal();

public:
	FORCEINLINE JCamera* GetCurrentMainCam() const { return mCurrentCamera; }

public:
	void SetCurrentMainCam(JWTextView InName);

private:
	JCamera* mCurrentCamera;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MCameraManager>;
	friend class JWorld;

	MCameraManager();

public:
	MCameraManager(const MCameraManager&)            = delete;
	MCameraManager& operator=(const MCameraManager&) = delete;

#pragma endregion
};
