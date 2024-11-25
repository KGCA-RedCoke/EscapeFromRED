#include "MCameraManager.h"

MCameraManager::MCameraManager()
{
	Initialize_Internal();
}

void MCameraManager::Initialize_Internal()
{
	mCurrentCamera = CreateOrLoad<JCamera_Debug>("EditorCamera");
	assert(mCurrentCamera);
}

void MCameraManager::SetCurrentMainCam(JWTextView InName)
{}
