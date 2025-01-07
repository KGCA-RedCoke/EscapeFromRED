#include "JLevel_Main.h"


#include "Core/Entity/Audio/MSoundManager.h"
#include "Core/Entity/Navigation/BigGrid.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Window/Application.h"
#include "Game/Src/Player/APlayerCharacter.h"


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
			if (actor->GetChildComponentByType(NAME_COMPONENT_SPAWNER))
			{
				EnemySpawner.push_back(actor.get());
			}
			// actor->BeginPlay();
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

		// GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Player, ETraceType::Pawn, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Player, ETraceType::BlockingVolume, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Player, ETraceType::EnemyHitSpace, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Player, ETraceType::Interactive, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::BlockingVolume, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::PlayerWeapon, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::Pawn, true);

		G_NAV_MAP.Initialize();
		G_BIG_MAP.Initialize();
	});

	GetWorld.ThreadPool.ExecuteTask([this](){
		Utils::Serialization::DeSerialize("Game/Levels/Map.jasset", this);
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
