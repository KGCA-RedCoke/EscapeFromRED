#include "JLevel_Intro.h"

#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"

JLevel_Intro::JLevel_Intro()
{
	mWidgetComponents.reserve(1);

	mWidgetComponents.push_back(GetWorld.UIManager->Load("Game/UI/IntroScene.jasset"));

	mActors.clear();

	bThreadLoaded = true;

	mWidgetComponents[0]->mUIComponents[2]->OnAnimationEvent.Bind([&](float DeltaTime){
		auto& data   = mWidgetComponents[0]->mUIComponents[2]->GetInstanceData();
		data.Color.x = (sin(6.28318 * GetWorld.GetGameTime() / 3.f) + 1) / 2 + 0.173f;
	});
	mWidgetComponents[0]->mUIComponents[3]->OnAnimationEvent.Bind([&](float DeltaTime){
		auto& data   = mWidgetComponents[0]->mUIComponents[3]->GetInstanceData();
		data.Color.x = (sin(6.28318 * GetWorld.GetGameTime() / 5.f) + 1) / 2 + 0.173f;
	});
	mWidgetComponents[0]->mUIComponents[7]->OnAnimationEvent.Bind([&](float DeltaTime){
		auto& data   = mWidgetComponents[0]->mUIComponents[7]->GetInstanceData();
		data.Color.x = (sin(6.28318 * GetWorld.GetGameTime() / 10.f) + 1) / 2 + 0.173f;
	});

	mStartButtonSize = mWidgetComponents[0]->mUIComponents[5]->GetInstanceData().Size;
	mWidgetComponents[0]->mUIComponents[5]->OnAnimationEvent.Bind([&](float DeltaTime){
		auto& data   = mWidgetComponents[0]->mUIComponents[5]->GetInstanceData();
		data.Color.w = (sin(GetWorld.GetGameTime()) + 1) / 2;
		data.Size    = mStartButtonSize * FMath::Clamp((sin(GetWorld.GetGameTime()) + 1) / 2, 0.7f, 1.5f);
	});
}

JLevel_Intro::~JLevel_Intro() {}

void JLevel_Intro::InitializeLevel()
{}

void JLevel_Intro::UpdateLevel(float DeltaTime)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		JLevel* mainLevel = GetWorld.LevelManager->LoadMainLevel();
		GetWorld.LevelManager->SetActiveLevel(mainLevel);
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

void JLevel_Intro::RenderLevel()
{
	GetWorld.ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Alpha);

	for (auto& widget : mWidgetComponents)
	{
		widget->AddInstance();
	}
	MUIManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}
