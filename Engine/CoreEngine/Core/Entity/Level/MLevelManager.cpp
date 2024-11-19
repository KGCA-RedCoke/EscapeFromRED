#include "MLevelManager.h"

#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Graphics/Mesh/JMeshObject.h"

void MLevelManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								   void*        Entity)
{
	if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(), static_cast<JLevel*>(Entity));
	}
}

void MLevelManager::Update(float DeltaTime)
{
	if (mActiveLevel)
	{
		mActiveLevel->UpdateLevel(DeltaTime);
	}
}

void MLevelManager::Render()
{
	if (mActiveLevel)
	{
		mActiveLevel->RenderLevel();
	}
}

void MLevelManager::Destroy()
{}

JLevel* MLevelManager::CreateLevel(const JText& InSavePath)
{
	JLevel*           newLevel  = CreateOrLoad(InSavePath);
	JStaticMeshActor* skySphere = newLevel->CreateActor<JStaticMeshActor>("SkySphere", nullptr, Path_Mesh_Sphere);

	skySphere->SetMaterialInstance(MMaterialInstanceManager::Get().CreateOrLoad("Game/Materials/SkySphere.jasset"));

	skySphere->SetWorldScale({500.f, 500.f, 500.f});

	return newLevel;
}

void MLevelManager::SetActiveLevel(JLevel* InLevel)
{
	mActiveLevel = InLevel;
}

MLevelManager::MLevelManager() {}
MLevelManager::~MLevelManager() {}
