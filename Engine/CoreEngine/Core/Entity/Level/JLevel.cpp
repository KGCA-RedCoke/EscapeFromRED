﻿#include "JLevel.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Interface/JWorld.h"

JLevel::JLevel(const JText& InPath, bool bUseTree)
	: JObject(InPath)
{
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
		if (loadActor->DeSerialize_Implement(InFileStream))
		{
			loadActor->Initialize();
			mOcTree->Insert(loadActor.get());
			mActors.push_back(std::move(loadActor));
			
		}
	}

	return true;
}

void JLevel::InitializeLevel()
{
	mOcTree = MakeUPtr<Quad::JTree>();
	mOcTree->Initialize({{0, 0, 0}, {10000, 0, 10000}}, MAX_DEPTH);
}

void JLevel::UpdateLevel(float DeltaTime)
{
	std::erase_if(
				  mActors,
				  [&](UPtr<AActor>& actor){
					  actor->Tick(DeltaTime);
					  if (actor->IsPendingKill())
					  {
						  mOcTree->Remove(actor.get());
						  actor = nullptr;
						  return true;
					  }
					  return false;
				  });

	mOcTree->Update();
}

void JLevel::RenderLevel()
{
	auto* cam = GetWorld.CameraManager->GetCurrentMainCam();

	G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());

	// 옥트리 내부의 포함되는 액터들만 렌더링
	// OcTree의 노드가 카메라의 Frustum과 교차하는지 체크
	mOcTree->Render(GetWorld.CameraManager->GetCurrentMainCam());
	// for (const auto& actor : mActors)
	// {
	// 	actor->Draw();
	// }

	G_DebugBatch.Draw();

	G_DebugBatch.PostRender();

	MMeshManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
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
