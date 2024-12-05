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
	auto EditorViewport = GetWorld.ViewportManager->FetchResource(Name_Editor_Viewport);
	assert(EditorViewport);

	auto mainCam = MCameraManager::Get().GetCurrentMainCam();

	FVector topViewPos = mainCam->GetEyePositionVector();
	topViewPos.y += 1000.f;

	// FVector lookAt = FVector{topViewPos.x, 0, topViewPos.z};

	FVector downVector = {0, -1, 0};

	mCamera->SetViewParams(FVector(0, 1500.f, -1500.f), {0, 0, 1});
	mCamera->Tick(DeltaTime);

	// mSampleTexture = EditorViewport->SRV;
	// assert(mSampleTexture);
	ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());
}

void GUI_Camera_Debug_Frustum::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	GetWorld.ViewportManager->SetRenderTarget(mTitle.c_str());

	GetWorld.ShaderManager->UpdateCamera(mCamera);

	for (auto& actor : GetWorld.LevelManager->GetActiveLevel()->mActors)
	{
		actor.get()->Draw();
	}

	GetWorld.MeshManager->FlushCommandList(GetWorld.D3D11API->GetImmediateDeviceContext());
	// GetWorld.CameraManager->GetCurrentMainCam()->PreRender(GetWorld.D3D11API->GetImmediateDeviceContext());
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
