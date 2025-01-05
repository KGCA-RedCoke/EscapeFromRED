#include "GUI_Camera_Debug_Frustum.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Window/Application.h"

GUI_Camera_Debug_Frustum::GUI_Camera_Debug_Frustum(const JText& InTitle)
	: GUI_Base(InTitle)
{
	GUI_Camera_Debug_Frustum::Initialize();
}

void GUI_Camera_Debug_Frustum::Update_Implementation(float DeltaTime)
{

	ImGui::Image(GetWorld.DirectionalLightShadowMap->SRV.Get(), ImGui::GetContentRegionAvail());
}

void GUI_Camera_Debug_Frustum::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정

}

void GUI_Camera_Debug_Frustum::Initialize()
{
	// 1. 뷰포트를 생성
	mViewport = MViewportManager::Get().Load(mTitle, 1280, 720);
	assert(mViewport);

	// 2. 머티리얼 에디터 프리뷰에 사용될 카메라를 생성
	mCamera = GetWorld.CameraManager->Load("FrustumCamera");
	assert(mCamera);
}
