#include "MCameraManager.h"

void MCameraManager::Initialize()
{
	mCurrentCamera = CreateOrLoad(L"EditorCamera");
	assert(mCurrentCamera);
	mCurrentCamera->Initialize();
}

void MCameraManager::SetCurrentMainCam(JWTextView InName)
{}

void MCameraManager::SetCameraConstantBuffer(uint32_t InSlot) const
{
	assert(mCurrentCamera);
	mCurrentCamera->SetCameraConstantBuffer(InSlot);
}
