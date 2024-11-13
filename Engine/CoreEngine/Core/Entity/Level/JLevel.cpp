#include "JLevel.h"

#include "Core/Entity/Actor/JActor.h"

JLevel::JLevel(const JText& InName) {}

JLevel::~JLevel() {}

uint32_t JLevel::GetType() const
{
	return StringHash("Level");
}

bool JLevel::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Serialize Actors
	Utils::Serialization::Serialize_Container(mActors.data(), mActors.size(), FileStream);


	return true;
}

bool JLevel::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// DeSerialize Actors
	size_t actorSize;
	Utils::Serialization::DeSerialize_Primitive(&actorSize, sizeof(actorSize), InFileStream);
	// mActors.resize(actorSize);
	// for (int32_t i = 0; i < actorSize; ++i)
	// {
	// 	mActors[i] = new JActor();
	// 	mActors[i]->DeSerialize_Implement(InFileStream);
	// }


	return true;
}

void JLevel::AddActor(JActor* InActor)
{
	mActors.push_back(InActor);
}

void JLevel::LoadActorFromPath(const JText& InPath)
{
	// 파일에서 Load 시도 
	UPtr<JActor> tryLoadActor = MakeUPtr<JActor>();

	if (Utils::Serialization::DeSerialize(InPath.c_str(), tryLoadActor.get()))
	{
		mActors.push_back(tryLoadActor.get());
		// mChildObjs.push_back(std::move(tryLoadActor));
	}
	else
	{
		LOG_CORE_ERROR("Failed to Load Actor from Path: {}", InPath);
	}

}

JActor* JLevel::CreateActor(const JText& InName)
{
	// JActor* newActor = CreateDefaultSubObject<JActor>(InName);
	// mActors.push_back(newActor);

	return nullptr;
	// return newActor;
}
