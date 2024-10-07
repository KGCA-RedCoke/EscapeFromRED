#include "MCameraManager.h"

MCameraManager::MCameraManager()
{
	Initialize_Internal();
}

void MCameraManager::Initialize_Internal()
{
	mCurrentCamera = CreateOrLoad("EditorCamera");
	assert(mCurrentCamera);
}

void MCameraManager::SetCurrentMainCam(JWTextView InName)
{}

void MCameraManager::SetCameraConstantBuffer(uint32_t InSlot, JCamera* InCamera) const
{
	if (InCamera)
	{
		InCamera->SetCameraConstantBuffer(InSlot);
	}
	else
	{
		assert(mCurrentCamera);
		mCurrentCamera->SetCameraConstantBuffer(InSlot);
	}
}


