#include "JLevel_Main.h"

#include "Core/Entity/Audio/MSoundManager.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Window/Application.h"
#include "Game/Src/Player/APlayerCharacter.h"

JKihyunDialog::JKihyunDialog()
{
	AddUIComponent("Quest_Start_1");
	AddUIComponent("Quest_Start_2");
	AddUIComponent("Quest_Start_3");
	AddUIComponent("Quest_Start_4");
	AddUIComponent("Quest_Start_5");
	AddUIComponent("Quest_Start_6");
	AddUIComponent("Quest_Start_7");

	mUIComponents[0]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_1.png"));
	mUIComponents[1]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_2.png"));
	mUIComponents[2]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_3.png"));
	mUIComponents[3]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_4.png"));
	mUIComponents[4]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_5.png"));
	mUIComponents[5]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_6.png"));
	mUIComponents[6]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_7.png"));

	for (auto& component : mUIComponents)
	{
		component->SetColor({1, 1, 1, 1});
		component->SetSize({10, 5});
		component->SetVisible(false);
		component->SetPosition({0, -0.6});
	}
}

JLevel_Main::JLevel_Main()
{
	// for (int32_t i = 7; i < 10; ++i)
	// {
	// 	mHPBar[i - 7] = mWidgetComponents[0]->mUIComponents[i].get();
	// 	mHPBar[i - 7]->OnAnimationEvent.Bind([&](float DeltaTime){
	// 		auto& data  = mHPBar[i - 7]->GetInstanceData();
	// 		float alpha = data.Color.w;
	// 		alpha       = FMath::Lerp(alpha, 0.f, DeltaTime / 1.f);
	// 	});
	// }
	JLevel_Main::InitializeLevel();
}

JLevel_Main::~JLevel_Main() {}

void JLevel_Main::InitializeLevel()
{
	JLevel::InitializeLevel();

	mMainSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/MainSound.mp3");

	OnLevelLoaded.Bind([&](){

		for (auto& actor : mActors)
		{
			actor->BeginPlay();
		}
		// // Play 동작 처리
		mPlayerCharacter = GetWorld.SpawnActor<APlayerCharacter>("Player",
																 {-850.f, 250, -10000.f},
																 FVector::ZeroVector,
																 nullptr,
																 "Game/Mesh/SK_Hands_07.jasset");

		mPlayerCharacter->BeginPlay();

		mMainSound->Play(true);
		
		
		Application::s_AppInstance->LockMouseToWindow();

		bThreadLoaded = true;

		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::Pawn, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::BlockingVolume, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::PlayerWeapon, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::Ground, true);

		G_NAV_MAP.Initialize();
	});

	mKihyunDialog = MakeUPtr<JKihyunDialog>();

	mWidgetComponents.reserve(2);
	mWidgetComponents.push_back(GetWorld.UIManager->Load("Game/UI/NewWidget.jasset"));
	mWidgetComponents.push_back(mKihyunDialog.get());


	GetWorld.ThreadPool.ExecuteTask([this](){
		Utils::Serialization::DeSerialize("Game/Levels/Map.jasset", this);
	});

	OnQuestStart.Bind([&](uint32_t Index){
		mPlayerCharacter->LockInput(true);

		for (auto& component : mKihyunDialog->mUIComponents)
		{
			component->SetVisible(false);
		}
		mKihyunDialog->mUIComponents[Index]->SetVisible(true);

	});

	OnQuestEnd.Bind([&](uint32_t Index){
		for (auto& component : mKihyunDialog->mUIComponents)
		{
			component->SetVisible(false);
		}
		mPlayerCharacter->LockInput(false);
	});
}

void JLevel_Main::UpdateLevel(float DeltaTime)
{
	JLevel::UpdateLevel(DeltaTime);
}

void JLevel_Main::RenderLevel()
{
	JLevel::RenderLevel();
}
