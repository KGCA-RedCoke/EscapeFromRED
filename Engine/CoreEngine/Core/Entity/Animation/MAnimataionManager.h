#pragma once
#include "JAnimationClip.h"
#include "Core/Manager/Manager_Base.h"

class MAnimataionManager : public Manager_Base<JAnimationClip, MAnimataionManager>
{
public:
void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash, void* Entity) override;
private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MAnimataionManager>;

	MAnimataionManager();
	~MAnimataionManager();

public:
	MAnimataionManager(const MAnimataionManager&)            = delete;
	MAnimataionManager& operator=(const MAnimataionManager&) = delete;

#pragma endregion
};
