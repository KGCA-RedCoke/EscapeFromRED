#include "GUI_Editor_UI.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"

GUI_Editor_UI::GUI_Editor_UI(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{
	assert(mCamera);

	mCamera->SetViewParams({0, 0, -10}, {0, 0, 1});
	mCamera->SetProjParams(M_PI / 4, 1280.f / 720.f, 0.01f, 1.f);

	mViewport->OnViewportResized.UnBindAll();
	mViewport->OnViewportResized.Bind([&](uint32_t InWidth, uint32_t InHeight){
		mCamera->SetProjParams(M_PI / 4, static_cast<float>(InWidth) / static_cast<float>(InHeight), 0.01f, 1.f);
	});


	mWidgetComponent   = MUIManager::Get().Load(InTitle);
	auto* renderTarget = mWidgetComponent->GetRenderTarget();
	if (!renderTarget)
	{
		mWidgetComponent->SetRenderTarget(mViewport->RTV_2D.Get());
	}
}

void GUI_Editor_UI::ShowMenuBar()
{
	GUI_Editor_Base::ShowMenuBar();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Button"))
		{
			if (ImGui::MenuItem("Box"))
			{
				mWidgetComponent->AddUIComponent(std::format("BOX_{0}", mWidgetComponent->GetComponentSize()));
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save"))
		{
			Utils::Serialization::Serialize("Game/UI/NewWidget.jasset", mWidgetComponent);
		}

		ImGui::EndMenuBar();
	}
}

void GUI_Editor_UI::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);


	mWidgetComponent->Tick(DeltaTime);

	ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());


	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= ImGui::GetItemRectMin().x;
	mousePos.y -= ImGui::GetItemRectMin().y;
	mWidgetComponent->SetMousePos({mousePos.x, mousePos.y});

	// LOG_CORE_INFO("Mouse Pos : {0}, {1}", mousePos.x, mousePos.y);
	mWidgetComponent->AddInstance();


	if (ImGui::IsItemClicked(0))
	{
		mSelectedComponent = mWidgetComponent->GetClickedComponent({mousePos.x, mousePos.y},
																   {mWindowSize.x, mWindowSize.y});
	}

	ShowEditWindow();
}

void GUI_Editor_UI::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	MViewportManager::Get().SetRenderTarget(mTitle.c_str(), FLinearColor::Alpha);
	MShaderManager::Get().UpdateCamera(mCamera);
	MUIManager::Get().FlushCommanaaaaaaaaawwwwwwwwwwwwwwwwdList(G_Device.GetImmediateDeviceContext());
}

void GUI_Editor_UI::ShowEditWindow()
{
	if (mSelectedComponent)
	{
		mSelectedComponent->ShowEditor();
	}
}
