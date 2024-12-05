#include "GUI_Editor_UI.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

GUI_Editor_UI::GUI_Editor_UI(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{
	assert(mCamera);

	mCamera->SetViewParams({0, 0, -10}, {0, 0, 0});
}

void GUI_Editor_UI::ShowMenuBar()
{
	GUI_Editor_Base::ShowMenuBar();

	if (ImGui::BeginMenuBar())
	{

		ImGui::EndMenuBar();
	}
}

void GUI_Editor_UI::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);


	ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		if (mCamera)
		{
			mCamera->Tick(mDeltaTime);
		}
	}

}

void GUI_Editor_UI::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	MViewportManager::Get().SetRenderTarget(mTitle.c_str(), FLinearColor::Gallary);

	MShaderManager::Get().UpdateCamera(mCamera);

	G_DebugBatch.PreRender();
	G_DebugBatch.DrawGrid_Implement(
									{mWindowSize.x, 0},
									{0, mWindowSize.y},
									{0, 0},
									16,
									16,
									Colors::Blue
								   );
	G_DebugBatch.PostRender();
}
