#pragma once
#include "JActor.h"

/**
 * 액터매니저에서는 실제 레벨에 배치될 액터를 관리하는게 아니다
 * 액터매니저에서는 액터를 직렬화해서 템플릿으로 저장해놓는다.
 * 레벨에서 액터가 필요하면 템플릿에서 복사해서 사용한다.
 */
class MActorManager : public Manager_Base<JActor, MActorManager>
{
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash, void* Entity) override;
	
public:
	UPtr<JActor> CreateActor(const JText& InName);


#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MActorManager>;

	MActorManager();
	~MActorManager();

public:
	MActorManager(const MActorManager&)            = delete;
	MActorManager& operator=(const MActorManager&) = delete;
#pragma endregion
};
