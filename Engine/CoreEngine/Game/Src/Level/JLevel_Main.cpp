#include "JLevel_Main.h"

#include "Core/Entity/Navigation/NavTest.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Player/APlayerCharacter.h"

JLevel_Main::JLevel_Main()
{
	OnLevelLoaded.Bind([&](){
		// // Play 동작 처리
		APlayerCharacter* character = GetWorld.SpawnActor<APlayerCharacter>("Test Player",
																			FVector::ZeroVector,
																			FVector::ZeroVector,
																			nullptr,
																			"Game/Mesh/SK_Hands_07.jasset");
		character->BeginPlay();

		bThreadLoaded = true;

		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::Pawn, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::BlockingVolume, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::PlayerWeapon, true);
		GetWorld.ColliderManager->SetCollisionLayer(ETraceType::Pawn, ETraceType::Ground, true);

	});
	mWidgetComponents.reserve(1);
	mWidgetComponents.push_back(GetWorld.UIManager->Load("Game/UI/Crosshair.jasset"));

	JLevel_Main::InitializeLevel();
	G_NAV_MAP.Initialize();

	GetWorld.ThreadPool.ExecuteTask([this](){
		Utils::Serialization::DeSerialize("Game/Levels/Map.jasset", this);
	});
}

JLevel_Main::~JLevel_Main() {}

void JLevel_Main::InitializeLevel()
{
	JLevel::InitializeLevel();
}

void JLevel_Main::UpdateLevel(float DeltaTime)
{
	JLevel::UpdateLevel(DeltaTime);
}

void JLevel_Main::RenderLevel()
{
	JLevel::RenderLevel();
}
