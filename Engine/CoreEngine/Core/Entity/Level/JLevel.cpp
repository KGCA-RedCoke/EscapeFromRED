#include "JLevel.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"

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

		UPtr<JActor> loadActor = UPtrCast<JActor>(MClassFactory::Get().Create(objType));
		if (loadActor->DeSerialize_Implement(InFileStream))
		{
			mActors.push_back(std::move(loadActor));
		}
	}

	return true;
}

void JLevel::InitializeLevel()
{}

void JLevel::UpdateLevel(float DeltaTime)
{
	for (int32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->Tick(DeltaTime);
	}
	mActors.erase(std::ranges::remove_if(mActors,
										 [](const UPtr<JActor>& actor){ return actor->IsPendingKill(); }).begin(),
				  mActors.end());
}

void JLevel::RenderLevel()
{
	for (int32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->Draw();
	}

	MMeshManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}

// void JLevel::AddActor(const Ptr<JActor>& InActor)
// {
// 	mActors.push_back(InActor);
// }

JActor* JLevel::LoadActorFromPath(const JText& InPath)
{
	// // 파일에서 Load 시도 
	// Ptr<JActor> tryLoadActor = MakePtr<JActor>();
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
