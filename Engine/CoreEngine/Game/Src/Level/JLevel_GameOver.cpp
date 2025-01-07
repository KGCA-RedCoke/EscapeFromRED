#include "JLevel_GameOver.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"

JLevel_GameOver::JLevel_GameOver()
{
	mGameOverWidget = GetWorld.UIManager->Load("Game/UI/GameOverScene.jasset");
	mWidgetComponents.push_back(mGameOverWidget);

	mGameOverWidget->mUIComponents[0]->OnAnimationEvent.Bind([&](float DeltaTime){
		uint32_t normalBtnIndex   = mButtonIndex == 0 ? 3 : 2;
		uint32_t selectedBtnIndex = mButtonIndex == 0 ? 2 : 3;

		auto& data   = mGameOverWidget->mUIComponents[selectedBtnIndex]->GetInstanceData();
		data.Color.x = 1.f;
		data.Size    = FVector2{2.f, 2.f};

		auto& data2 = mGameOverWidget->mUIComponents[normalBtnIndex]->GetInstanceData();
		data2.Color.x *= 0.6f;
		data2.Size = FVector2{1.5f, 1.5f};
	});

	bThreadLoaded = true;

}

JLevel_GameOver::~JLevel_GameOver() {}

void JLevel_GameOver::InitializeLevel()
{}

void JLevel_GameOver::UpdateLevel(float DeltaTime)
{
	if (mGameOverWidget->IsVisible())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			if (mButtonIndex == 0)
			{
				GetWorld.LevelManager->SetActiveLevel(GetWorld.LevelManager->LoadMainLevel());
			}
			else
			{
				PostQuitMessage(0);
			}

			return;
		}

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			mButtonIndex = 0;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			mButtonIndex = 1;
		}
	}

	std::erase_if(
			  mWidgetComponents,
			  [&](JWidgetComponent* widget){

				  widget->Tick(DeltaTime);
				  if (widget->IsPendingKill())
				  {
					  widget = nullptr;
					  return true;
				  }
				  return false;
			  });
}

void JLevel_GameOver::RenderLevel()
{
	GetWorld.ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Alpha);

	for (auto& widget : mWidgetComponents)
	{
		widget->AddInstance();
	}
	MUIManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}
