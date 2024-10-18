#pragma once
#include "JCamera.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/Manager_Base.h"

class MCameraManager : public Manager_Base<JCamera, MCameraManager>
{
private:
	void Initialize_Internal();

public:
	FORCEINLINE Ptr<JCamera> GetCurrentMainCam() const { return mCurrentCamera; }

public:
	void SetCurrentMainCam(JWTextView InName);

public:
	void SetCameraConstantBuffer(uint32_t InSlot = CBuffer::SLOT_CAMERA, JCamera* InCamera = nullptr) const;

private:
	Ptr<JCamera> mCurrentCamera;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MCameraManager>;
	friend class MManagerInterface;

	MCameraManager();

public:
	MCameraManager(const MCameraManager&)            = delete;
	MCameraManager& operator=(const MCameraManager&) = delete;

#pragma endregion
};
