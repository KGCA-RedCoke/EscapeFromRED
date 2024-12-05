#include "MCameraManager.h"

MCameraManager::MCameraManager()
{
	Initialize_Internal();
}

void MCameraManager::Initialize_Internal()
{
	mCurrentCamera = Load<JCamera_Debug>("EditorCamera");
	assert(mCurrentCamera);
}

void MCameraManager::SetCurrentMainCam(JCameraComponent* InCamera)
{
	mCurrentCamera = InCamera;
}
