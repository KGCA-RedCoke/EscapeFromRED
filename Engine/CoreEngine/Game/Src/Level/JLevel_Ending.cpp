#include "JLevel_Ending.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"

JLevel_Ending::JLevel_Ending()
{
	mEndingWidget = GetWorld.UIManager->Load("Game/UI/EndScene.jasset");
	mWidgetComponents.push_back(mEndingWidget);

	bThreadLoaded = true;
}

JLevel_Ending::~JLevel_Ending() {}

void JLevel_Ending::InitializeLevel()
{}

void JLevel_Ending::UpdateLevel(float DeltaTime)
{

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{

		PostQuitMessage(0);
		return;
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

void JLevel_Ending::RenderLevel()
{
	GetWorld.ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Alpha);

	for (auto& widget : mWidgetComponents)
	{
		widget->AddInstance();
	}
	MUIManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}
