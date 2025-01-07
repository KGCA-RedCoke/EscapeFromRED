#include "MLevelManager.h"

#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Level/JLevel_Intro.h"
#include "Game/Src/Level/JLevel_Main.h"

void MLevelManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								   void*        Entity)
{
	JLevel* loadLevel = static_cast<JLevel*>(Entity);

	if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
	{
		GetWorld.ThreadPool.ExecuteTask([OriginalNameOrPath, loadLevel](){
			Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(), loadLevel);
		});
	}
}

void MLevelManager::Update(float DeltaTime)
{
	if (mActiveLevel && mActiveLevel->IsLoaded())
	{
		mActiveLevel->UpdateLevel(DeltaTime);
	}
}

void MLevelManager::Render()
{
	if (mActiveLevel)
	{
		if (!mActiveLevel->IsLoaded())
		{
			MUIManager::Get().LoadingScreen->Tick(0);
			MUIManager::Get().LoadingScreen->AddInstance();
			MUIManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
		}
		else
		{
			mActiveLevel->RenderLevel();
		}
	}
}

void MLevelManager::Destroy()
{}

JLevel* MLevelManager::LoadIntroLevel()
{
	uint32_t hash       = StringHash("IntroLevel");
	auto     introLevel = MakeUPtr<JLevel_Intro>();
	auto*    ptr        = introLevel.get();
	mManagedList[hash]  = std::move(introLevel);

	return ptr;
}

JLevel* MLevelManager::LoadMainLevel()
{
	uint32_t hash      = StringHash("MainLevel");
	auto     mainLevel = MakeUPtr<JLevel_Main>();
	auto*    ptr       = mainLevel.get();
	mManagedList[hash] = std::move(mainLevel);

	return ptr;
}

JLevel* MLevelManager::CreateLevel(const JText& InSavePath)
{
	JLevel* newLevel = Load(InSavePath);
	auto*   plane    = newLevel->CreateActor<JStaticMeshActor>("Ground", "Game/Mesh/Plane.jasset");
	plane->SetLocalScale({1000, 1, 1000});
	// plane->SetLocalRotation({-90, 0, 0});
	plane->SetWorldLocation({0, -10, 0});

	return newLevel;
}

void MLevelManager::SetActiveLevel(JLevel* InLevel)
{
	mActiveLevel = InLevel;
	MCollisionManager::Get().UnEnrollAllCollision();
}

MLevelManager::MLevelManager() {}
MLevelManager::~MLevelManager() {}
