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
