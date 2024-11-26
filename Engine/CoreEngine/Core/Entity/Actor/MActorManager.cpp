#include "MActorManager.h"

void MActorManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								   void*        Entity)
{
	// 액터 파일 경로가 존재하고 jasset 파일이라면 데이터를 로드한다.
	if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
	{
		AActor* actor = static_cast<AActor*>(Entity);

		if (!Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(), actor))
		{
			LOG_CORE_ERROR("Failed to load actor object(Invalid Path maybe...): {0}", OriginalNameOrPath);
		}
	}
}

UPtr<AActor> MActorManager::CreateActor(const JText& InName)
{
	uint32_t hash = StringHash(InName.c_str());

	auto it = mManagedList.find(hash);

	if (it != mManagedList.end())
	{
		UPtr<IManagedInterface> clone = it->second->Clone();

		return UPtrCast<AActor>(std::move(clone));
	}

	return nullptr;
}

MActorManager::MActorManager() {}
MActorManager::~MActorManager() {}
