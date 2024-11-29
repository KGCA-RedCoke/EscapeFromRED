#include "JLevel.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/SpaceDivision/SpaceDivision.h"

JLevel::JLevel(const JText& InPath)
	: mName(ParseFile(InPath))
{
	mPath = InPath;
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
		if (loadActor->DeSerialize_Implement(InFileStream))
		{
			// mOcTree->Insert(loadActor.get());
			mActors.push_back(std::move(loadActor));
		}
	}

	return true;
}

void JLevel::InitializeLevel()
{
	// mOcTree = MakeUPtr<Oc::JTree>();
	// mOcTree->Initialize({{0, 0, 0}, {5000, 5000, 5000}}, 5);
}

void JLevel::UpdateLevel(float DeltaTime)
{
	std::erase_if(
				  mActors,
				  [&](const UPtr<AActor>& actor){
					  actor->Tick(DeltaTime);
					  return actor->IsPendingKill();
				  });


	// OcTree Update
	// mOcTree->Update();
}

void JLevel::RenderLevel()
{
	G_DebugBatch.PreRender();


	for (int32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->Draw();
	}

	G_DebugBatch.Draw();

	G_DebugBatch.PostRender();

	MMeshManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());

	// GetWorld.CameraManager->GetCurrentMainCam()->PreRender(G_Device.GetImmediateDeviceContext());
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
