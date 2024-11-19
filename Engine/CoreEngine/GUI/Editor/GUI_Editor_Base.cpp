#include "GUI_Editor_Base.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

GUI_Editor_Base::GUI_Editor_Base(const JText& InTitle)
	: GUI_Base(InTitle),
	  mDeltaTime(0)
{
	mWindowFlags |= ImGuiWindowFlags_MenuBar;

	// 1. 뷰포트를 생성
	mViewport = MViewportManager::Get().CreateOrLoad(mTitle, 1280, 720);
	assert(mViewport);

	// 2. 머티리얼 에디터 프리뷰에 사용될 카메라를 생성
	mCamera = MCameraManager::Get().CreateOrLoad(InTitle);
	assert(mCamera);
}

void GUI_Editor_Base::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	MViewportManager::Get().SetRenderTarget(mTitle.c_str());

	MShaderManager::Get().UpdateCamera(mCamera);
}


void GUI_Editor_Base::Update_Implementation(float DeltaTime)
{
	mDeltaTime = DeltaTime;

	mWindowSize = ImGui::GetContentRegionAvail();
}


void GUI_Editor_Base::ShowMenuBar()
{
	GUI_Base::ShowMenuBar();
}
