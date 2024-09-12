#include "common_pch.h"
#include "MCameraManager.h"

void MCameraManager::Initialize()
{
	mCurrentCamera = CreateOrLoad(L"EditorCamera");
	assert(mCurrentCamera);
	mCurrentCamera->Initialize();
}

void MCameraManager::SetCurrentMainCam(JWTextView InName) {}
