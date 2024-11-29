#pragma once
#include "JCameraComponent.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/Manager_Base.h"

class MCameraManager : public Manager_Base<JCameraComponent, MCameraManager>
{
private:
	void Initialize_Internal();

public:
	FORCEINLINE JCameraComponent* GetCurrentMainCam() const { return mCurrentCamera; }

public:
	void SetCurrentMainCam(JCameraComponent* InName);

private:
	JCameraComponent* mCurrentCamera;

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
