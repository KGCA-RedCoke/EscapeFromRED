#include "JLevel.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Audio/MSoundManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Player/APlayerCharacter.h"
#include "GUI/MGUIManager.h"

JKihyunDialog::JKihyunDialog()
{
	AddUIComponent("Quest_Start_1");
	AddUIComponent("Quest_Start_2");
	AddUIComponent("Quest_Start_3");
	AddUIComponent("Quest_Start_4");
	AddUIComponent("Quest_Start_5");
	AddUIComponent("Quest_Start_6");
	AddUIComponent("Quest_Start_7");
	AddUIComponent("Quest_End_1");
	AddUIComponent("Quest_End_2");
	AddUIComponent("Quest_End_3");
	AddUIComponent("Quest_End_4");
	AddUIComponent("Quest_End_5");

	mUIComponents[0]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_1.png"));
	mUIComponents[1]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_2.png"));
	mUIComponents[2]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_3.png"));
	mUIComponents[3]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_4.png"));
	mUIComponents[4]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_5.png"));
	mUIComponents[5]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_6.png"));
	mUIComponents[6]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_Start_7.png"));

	mUIComponents[7]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_End_1.png"));
	mUIComponents[8]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_End_2.png"));
	mUIComponents[9]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_End_3.png"));
	mUIComponents[10]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_End_4.png"));
	mUIComponents[11]->SetTexture(GetWorld.TextureManager->Load("Game/Textures/Dialog/Quest_End_5.png"));

	for (auto& component : mUIComponents)
	{
		component->SetColor({1, 1, 1, 1});
		component->SetSize({10, 5});
		component->SetVisible(false);
		component->SetPosition({0, -0.6});
	}
}

JKihyunDialog::~JKihyunDialog() {}

JLevel::JLevel() {}

JLevel::JLevel(const JText& InPath, bool bUseTree)
	: JObject(InPath)
{
	OnLevelLoaded.Bind([&](){
		bThreadLoaded = true;
	});

	if (bUseTree)
	{
		JLevel::InitializeLevel();
	}
}

JLevel::~JLevel() {}

uint32_t JLevel::GetType() const
{
	return HASH_ASSET_TYPE_LEVEL;
}

bool JLevel::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Serialize Actors
	size_t actorSize = mActors.size();
	Utils::Serialization::Serialize_Primitive(&actorSize, sizeof(actorSize), FileStream);
	for (int32_t i = 0; i < actorSize; ++i)
	{
		JText objType = mActors[i]->GetObjectType();
		Utils::Serialization::Serialize_Text(objType, FileStream);

		mActors[i]->Serialize_Implement(FileStream);
	}

	return true;
}

bool JLevel::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// DeSerialize Actors
	size_t actorSize;
	Utils::Serialization::DeSerialize_Primitive(&actorSize, sizeof(actorSize), InFileStream);
	mActors.reserve(actorSize);

	for (int32_t i = 0; i < actorSize; ++i)
	{
		JText objType;
		Utils::Serialization::DeSerialize_Text(objType, InFileStream);
		UPtr<AActor> loadActor = UPtrCast<AActor>(MClassFactory::Get().Create(objType));
		loadActor->DeSerialize_Implement(InFileStream);
		mActors.push_back(std::move(loadActor));
	}

	LOG_CORE_INFO("Current File Pos: {}", (int)InFileStream.tellg());


	// size_t widgetSize;
	// Utils::Serialization::DeSerialize_Primitive(&widgetSize, sizeof(widgetSize), InFileStream);
	// for (int32_t i = 0; i < widgetSize; ++i)
	// {
	// 	JText path;
	// 	Utils::Serialization::DeSerialize_Text(path, InFileStream);
	// 	// mWidgetComponents.push_back(MUIManager::Get().Load(path));
	// }

	for (auto& actor : mActors)
	{
		actor->Initialize();
		mOcTree->Insert(actor.get());
	}

	OnLevelLoaded.Execute();
	return true;
}

void JLevel::InitializeLevel()
{
	mOcTree = MakeUPtr<Quad::JTree>();
	mOcTree->Initialize({{0, 0, 0}, {10000, 5000, 10000}}, MAX_DEPTH);

	mWidgetComponents.reserve(4);
	mWidgetComponents.push_back(GetWorld.UIManager->Load("Game/UI/NewWidget.jasset"));


	auto eKeyUI = MakeUPtr<JUIComponent>("PressEKey");
	mPressEKey  = eKeyUI.get();
	mWidgetComponents[0]->mUIComponents.push_back(std::move(eKeyUI));
	mPressEKey = mWidgetComponents[0]->mUIComponents[0].get();
	mPressEKey->SetVisible(false);
	mPressEKey->SetPosition({0, -0.65});
	mPressEKey->SetColor({1, 1, 1, 1});
	mPressEKey->SetTexture(GetWorld.TextureManager->Load("Game/Textures/UI/Keyboard_E.PNG"));
	mPressEKey->OnAnimationEvent.Bind([&](float DeltaTime){
		auto& data = mPressEKey->GetInstanceData();
		data.Size  = FVector2::UnitVector * FMath::Clamp((sin(GetWorld.GetGameTime() * 5.f) + 1) * 2, 1.f, 1.5f);
	});

	for (int32_t i = 8; i <= 10; ++i)
	{
		mHPBar[i - 8] = mWidgetComponents[0]->mUIComponents[i].get();
		SetHPBar(i - 8, false);
	}

	mKihyunDialog = MakeUPtr<JKihyunDialog>();
	mWidgetComponents.push_back(mKihyunDialog.get());

	OnQuestStart.Bind([&](uint32_t Index){
		mPlayerCharacter->LockInput(true);

		mKihyunDialog->SetVisible(true);
		for (auto& component : mKihyunDialog->mUIComponents)
		{
			component->SetVisible(false);
		}
		mKihyunDialog->mUIComponents[Index]->SetVisible(true);

	});

	OnQuestEnd.Bind([&](uint32_t Index){
		mKihyunDialog->SetVisible(false);
		for (auto& component : mKihyunDialog->mUIComponents)
		{
			component->SetVisible(false);
		}
		mPlayerCharacter->LockInput(false);
	});
}

void JLevel::UpdateLevel(float DeltaTime)
{
	if (!bThreadLoaded)
	{
		return;
	}


	if (GetAsyncKeyState('P') & 0x8000)
	{
		mPlayerCharacter->OnPlayerHealthChanged.Execute(false);
	}

	std::erase_if(
				  mActors,
				  [&](UPtr<AActor>& actor){
					  // if (actor->IsValid())
					  // {
					  actor->Tick(DeltaTime);
					  // }

					  if (actor->IsPendingKill())
					  {
						  mOcTree->Remove(actor.get());

						  if (actor->IsPoolObject())
						  {
							  // 일단 따로 빼놓고 나중에 처리
							  mEnemyPool.DeSpawn(std::move(UPtrCast<AEnemy>(std::move(actor))));
						  }

						  return true;
					  }
					  return false;
				  });

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

	// 예약된 액터들을 추가
	for (auto& actor : mReservedActors)
	{
		mOcTree->Insert(actor.get());
		mActors.push_back(std::move(actor));
	}

	mReservedActors.clear();

	mOcTree->Update();
}

void JLevel::RenderLevel()
{
	auto* cam = GetWorld.CameraManager->GetCurrentMainCam();

	GetWorld.ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Alpha);

	G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());

	// 옥트리 내부의 포함되는 액터들만 렌더링
	// OcTree의 노드가 카메라의 Frustum과 교차하는지 체크

	mOcTree->Render(GetWorld.CameraManager->GetCurrentMainCam());
\
	G_DebugBatch.Draw();

	G_DebugBatch.PostRender();

	MMeshManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());

	for (auto& widget : mWidgetComponents)
	{
		widget->AddInstance();
		// MUIManager::Get().Load("Game/UI/NewWidget.jasset")->AddInstance();
	}
	MUIManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}

// void JLevel::AddActor(const Ptr<AActor>& InActor)
// {
// 	mActors.push_back(InActor);
// }

AActor* JLevel::LoadActorFromPath(const JText& InPath)
{
	// // 파일에서 Load 시도 
	// Ptr<AActor> tryLoadActor = MakePtr<AActor>();
	//
	// if (Utils::Serialization::DeSerialize(InPath.c_str(), tryLoadActor.get()))
	// {
	// 	mActors.push_back(tryLoadActor);
	// }
	// else
	// {
	// 	LOG_CORE_ERROR("Failed to Load Actor from Path: {}", InPath);
	// 	tryLoadActor = nullptr;
	// }
	//
	// return tryLoadActor;
	return nullptr;
}

void JLevel::ClearLevel()
{
	// mActors.clear();
	// mReservedActors.clear();
	// mActorIndexMap.clear();
	// mOcTree->Clear();
	// MCollisionManager::Get().UnEnrollAllCollision();
	// mPlayerCharacter = nullptr;

	GetWorld.LevelManager->SetActiveLevel(GetWorld.LevelManager->LoadGameOverLevel());
}

void JLevel::ShowPressEKey(bool bShow) { mPressEKey->SetVisible(bShow); }

void JLevel::SetHPBar(uint32_t InIndex, bool bShow) const
{
	mHPBar[InIndex]->SetVisible(bShow);
}

void JLevel::IncreaseHPBar(uint32_t InIndex)
{
	LOG_CORE_INFO("Player Health Increased");
	SetHPBar(InIndex - 1, true);

}

void JLevel::DecreamentHPBar(uint32_t InIndex)
{
	LOG_CORE_INFO("Player Health Decreased");
	SetHPBar(InIndex, false);
}
