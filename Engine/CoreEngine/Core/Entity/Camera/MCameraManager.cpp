#include "MCameraManager.h"

#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"

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
	G_DebugBatch.SetCamera(mCurrentCamera);
}
